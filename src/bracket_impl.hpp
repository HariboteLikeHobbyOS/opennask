#ifndef BRACKET_IMPL_HPP_
#define BRACKET_IMPL_HPP_

#include "nask_utility.hpp"
#include "bracket_utility.hpp"

namespace nask_utility {

     /**
      * Osaskのアセンブラに独自にある以下のようなテキストを処理する
      * [FORMAT "WCOFF"], [FILE "xxxx.c"], [INSTRSET "i386"]
      */
     int Instructions::process_token_BRACKET(TParaTokenizer& tokenizer, VECTOR_BINOUT& binout_container) {

	  for (TParaToken token = tokenizer.Next(); ; token = tokenizer.Next()) {
	       if (token.Is("[") && tokenizer.LookAhead(1).Is("FORMAT")) {
		    this->exists_section_table = true;
		    log()->info("process FORMAT as {}", tokenizer.LookAhead(2).AsString());

		    PIMAGE_FILE_HEADER header = {};
		    const std::string target = tokenizer.LookAhead(2).AsString();
		    process_format_statement(header, target, binout_container);
		    return 0;

	       } else if (token.Is("[") && tokenizer.LookAhead(1).Is("INSTRSET")) {

		    std::string cpu = tokenizer.LookAhead(2).AsString();
		    cpu = cpu.erase(0, 1);
		    cpu = cpu.erase(cpu.size() - 1);

		    if (SUPPORT_CPUS.count(cpu) == 0) {
			 std::cerr << "NASK : INSTRSET syntax error, "
				   << cpu
				   << " is not supported"
				   << std::endl;
			 return 17;
		    } else {
			 this->support = SUPPORT_CPUS.at(cpu.c_str());
			 log()->info("process INSTRSET as {}", this->support_cpus[this->support]);
			 return 0;
		    }

	       } else if (token.Is("[") && tokenizer.LookAhead(1).Is("FILE")) {
		    // ".file"のフィールドの書き込みは、
		    // 通常のオペコードの書き込みが終了したあとに行う必要があるようだ
		    log()->info("process {}", tokenizer.LookAhead(1).AsString());
		    log()->info("process bracket {}", tokenizer.LookAhead(2).AsString());

		    // auxiliary element ".file"
		    std::string file_name = tokenizer.LookAhead(2).AsString();
		    file_name = file_name.erase(0, 1);
		    file_name = file_name.erase(file_name.size() - 1);

		    this->file_auxiliary_name = file_name;
		    this->exists_file_auxiliary = true;
		    this->file_auxiliary = {
			 { '.', 'f', 'i', 'l', 'e', 0, 0, 0 /* shortName */ },
			 0x00000000,
			 0xfffe,
			 0x0000,
			 0x67, 0x01
		    };
	       }
	       break;
	  }

	  return 0;
     }

     void process_format_statement(PIMAGE_FILE_HEADER& header, const std::string& target, VECTOR_BINOUT& binout_container) {

	  if (target == "\"WCOFF\"") {
	       log()->info("target: {}, process as Portable Executable", target);
	       header.machine              = I386MAGIC;
	       header.numberOfSections     = 0x0003;
	       header.pointerToSymbolTable = 0x00000000; /* symboltable		   */
	       header.numberOfSymbols	   = 0x00000000; /* number of symbols 8+α */

	       auto ptr = reinterpret_cast<uint8_t*>(&header);
	       auto buffer = std::vector<uint8_t>{ ptr, ptr + sizeof(header) };
	       std::copy(buffer.begin(), buffer.end(), back_inserter(binout_container));

	       // section table ".text"
	       PIMAGE_SECTION_HEADER text = {
		    { '.', 't', 'e', 'x', 't', 0, 0, 0 /* name */ },
		    0x00000000, // Misc
		    0x00000000, // VirtualAddress
		    0x00000002,	// SizeOfRawData
		    0x0000008c,	// PointerToRawData
		    0x0000008e,	// PointerToRelocations <-- 可変
		    0x00000000,	// PointerToLinenumbers
		    0x0000,	// NumberOfRelocations
		    0x0000,	// NumberOfLinenumbers
		    0x60100020  /* +0x38: flags, default_align = 1 */
	       };

	       auto text_buffer = create_buffer(text);
	       std::copy(text_buffer.begin(), text_buffer.end(), back_inserter(binout_container));

	       // section table ".data"
	       PIMAGE_SECTION_HEADER data = {
		    { '.', 'd', 'a', 't', 'a', 0, 0, 0 /* name */ },
		    0x00000000, // Misc
		    0x00000000, // VirtualAddress
		    0x00000000,	// SizeOfRawData
		    0x00000000,	// PointerToRawData
		    0x00000000,	// PointerToRelocations
		    0x00000000,	// PointerToLinenumbers
		    0x0000,	// NumberOfRelocations
		    0x0000,	// NumberOfLinenumbers
		    0xc0100040  /* +0x60: flags, default_align = 1 */
	       };

	       auto data_buffer = create_buffer(data);
	       std::copy(data_buffer.begin(), data_buffer.end(), back_inserter(binout_container));

	       // section table ".bss"
	       PIMAGE_SECTION_HEADER bss = {
		    { '.', 'b', 's', 's', 0, 0, 0, 0 /* name */ },
		    0x00000000, // Misc
		    0x00000000, // VirtualAddress
		    0x00000000,	// SizeOfRawData
		    0x00000000,	// PointerToRawData
		    0x00000000,	// PointerToRelocations
		    0x00000000,	// PointerToLinenumbers
		    0x0000,	// NumberOfRelocations
		    0x0000,	// NumberOfLinenumbers
		    0xc0100080  /* +0x88: flags, default_align = 1 */
	       };

	       auto bss_buffer = create_buffer(bss);
	       std::copy(bss_buffer.begin(), bss_buffer.end(), back_inserter(binout_container));
	       log()->info("Wrote '.text', '.data', '.bss' fields for Portable Executable");
	  }
     }

     void process_section_table(Instructions& inst, VECTOR_BINOUT& binout_container) {

	  // COFFヘッダのシンボルテーブルへのオフセットが確定
	  const uint32_t offset = binout_container.size();
	  log()->info("COFF file header's PointerToSymbolTable: 0x{:02x}", binout_container.size());
	  set_dword_into_binout(offset, binout_container, false, 8);
	  log()->info("section table '.text' PointerToSymbolTable: 0x{:02x}", binout_container.size());
	  set_dword_into_binout(offset, binout_container, false, sizeof(PIMAGE_FILE_HEADER) + 24);

	  // auxiliary element ".file"
	  if (inst.exists_file_auxiliary) {
	       auto ptr = reinterpret_cast<uint8_t*>(&inst.file_auxiliary);
	       auto buffer = std::vector<uint8_t>{ ptr, ptr + sizeof(inst.file_auxiliary) };
	       std::copy(buffer.begin(), buffer.end(), back_inserter(binout_container));

	       for ( size_t i = 0; i < 18; i++ ) {
		    if ( inst.file_auxiliary_name.size() <= i ) {
			 binout_container.push_back(0x00);
		    } else {
			 binout_container.push_back(inst.file_auxiliary_name.at(i));
		    }
	       }
	       log()->info("Wrote a '.file' auxiliary field for Portable Executable");
	  }

	  // element ".text"
	  PIMAGE_SYMBOL text = {
	       { '.', 't', 'e', 'x', 't', 0, 0, 0 /* shortName */ },
	       0x00000000,
	       WCOFF_TEXT_FIELD,
	       0x0000,
	       0x03, 0x01
	  };

	  auto text_buffer = create_buffer(text);
	  std::copy(text_buffer.begin(), text_buffer.end(), back_inserter(binout_container));
	  for ( size_t i = 0; i < 18; i++ ) {
	       if (i == 0) {
		    // FIXME: ここの0x02が意味わからない
		    binout_container.push_back(0x02);
	       } else {
		    binout_container.push_back(0x00);
	       }
	  }

	  // element ".data"
	  PIMAGE_SYMBOL data = {
	       { '.', 'd', 'a', 't', 'a', 0, 0, 0 /* shortName */ },
	       0x00000000,
	       WCOFF_DATA_FIELD,
	       0x0000,
	       0x03, 0x01
	  };

	  auto data_buffer = create_buffer(data);
	  std::copy(data_buffer.begin(), data_buffer.end(), back_inserter(binout_container));
	  for ( size_t i = 0; i < 18; i++ ) {
	       binout_container.push_back(0x00);
	  }

	  // element ".text"
	  PIMAGE_SYMBOL bss = {
	       { '.', 'b', 's', 's', 0, 0, 0, 0 /* shortName */ },
	       0x00000000,
	       WCOFF_BSS_FIELD,
	       0x0000,
	       0x03, 0x01
	  };

	  auto bss_buffer = create_buffer(bss);
	  std::copy(bss_buffer.begin(), bss_buffer.end(), back_inserter(binout_container));
	  for ( size_t i = 0; i < 18; i++ ) {
	       binout_container.push_back(0x00);
	  }

	  // セクションデータのサイズをもっておく
	  size_t section_data_size = 0;

	  // シンボル数を確定させる
	  log()->info("COFF file header's NumberOfSymbols: 0x{:02x}", 8 + inst.symbol_list.size());
	  const uint32_t number_of_symbols = 8 + inst.symbol_list.size();
	  set_dword_into_binout(number_of_symbols, binout_container, false, 12);

	  for ( std::string symbol_name : inst.symbol_list ) {
	       // 関数などのシンボル情報を書き込む
	       if (symbol_name.size() <= 8) {
		    section_data_size += symbol_name.size();
		    // 8byte以下の場合の処理しか作ってない
		    // シンボル名は （アセンブラ）_io_hlt => （イメージファイル）io_hlt で登録する
		    // FIXME: 整合性のためにそうしようとしたが、たぶん「_」つきがただしい
		    const std::string real_symbol_name = (starts_with(symbol_name, "_")) ?
			 symbol_name.substr(1, symbol_name.size()) : symbol_name;

		    log()->info("write short symbol name: {}", real_symbol_name);

		    PIMAGE_SYMBOL func = {
			 { 0, 0, 0, 0, 0, 0, 0, 0 /* shortName */ },
			 0x00000000,
			 WCOFF_TEXT_FIELD, // <-- 関数が実際どこのsectionにあるか
			 0x0000,
			 0x02, 0x00
		    };

		    std::copy(real_symbol_name.begin(),
			      real_symbol_name.end(),
			      func.shortName);

		    auto fn_buffer = create_buffer(func);
		    std::copy(fn_buffer.begin(), fn_buffer.end(), back_inserter(binout_container));

	       } else {
		    // 8byte以上
		    section_data_size += symbol_name.size();
		    const std::string real_symbol_name = (starts_with(symbol_name, "_")) ?
			 symbol_name.substr(1, symbol_name.size()) : symbol_name;

		    log()->info("write short symbol name: {}", real_symbol_name);

		    // まずテーブルサイズ
		    set_dword_into_binout(0x10, binout_container);
		    std::string symbol_name_hex = string_to_hex_no_notate(real_symbol_name);
		    const std::string symbols = trim(symbol_name_hex);

		    set_hexstring_into_binout(symbols, binout_container, false);
		    log()->info("section data size: {}", section_data_size);
		    return;
	       }
	  }

	  log()->info("section data size: {}", section_data_size);

	  binout_container.push_back(0x04);
	  binout_container.push_back(0x00);
	  binout_container.push_back(0x00);
	  binout_container.push_back(0x00);
     }

     std::vector<uint8_t> create_buffer(PIMAGE_SYMBOL& symbol) {
	  auto ptr = reinterpret_cast<uint8_t*>(&symbol);
	  auto buffer = std::vector<uint8_t>{ ptr, ptr + sizeof(symbol) };
	  return buffer;
     }

     std::vector<uint8_t> create_buffer(PIMAGE_SECTION_HEADER& symbol) {
	  auto ptr = reinterpret_cast<uint8_t*>(&symbol);
	  auto buffer = std::vector<uint8_t>{ ptr, ptr + sizeof(symbol) };
	  return buffer;
     }
}

#endif /* BRACKET_IMPL_HPP_ */
