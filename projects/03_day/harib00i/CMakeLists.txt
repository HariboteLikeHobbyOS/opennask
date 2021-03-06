#----------------------------------------------------------
message(STATUS "Entering directory projects/03_day/harib00i/")

set(NASK ${root_BINARY_DIR}/src/opennask)
set(FONT ${root_BINARY_DIR}/src/makefont)
set(B2O  ${root_BINARY_DIR}/src/bin2obj)
set(CONV ${root_BINARY_DIR}/objconv/objconv)
set(03_day_harib00i_OS    ${root_BINARY_DIR}/projects/03_day/harib00i/os.img)
set(03_day_harib00i_SYS	  ${root_BINARY_DIR}/projects/03_day/harib00i/os.sys)
set(03_day_harib00i_IPLB  ${root_BINARY_DIR}/projects/03_day/harib00i/ipl.bin)
set(03_day_harib00i_IPLS  ${root_SOURCE_DIR}/projects/03_day/harib00i/ipl10.nas)
set(03_day_harib00i_HEADB ${root_BINARY_DIR}/projects/03_day/harib00i/asmhead.bin)
set(03_day_harib00i_HEADS ${root_SOURCE_DIR}/projects/03_day/harib00i/asmhead.nas)
set(03_day_harib00i_BOOTB ${root_BINARY_DIR}/projects/03_day/harib00i/boot.bin)
set(03_day_harib00i_CCO	  ${root_BINARY_DIR}/projects/03_day/harib00i/boot.o)
set(03_day_harib00i_CCS	  ${root_SOURCE_DIR}/projects/03_day/harib00i/*.c)
set(03_day_harib00i_LDS	  ${root_SOURCE_DIR}/projects/os.lds)
set(03_day_harib00i_WILDOBJ ${root_BINARY_DIR}/projects/03_day/harib00i/*.o)


add_custom_target(03_day_harib00i_run
  COMMAND ${QEMU} ${QEMUOPT} ${03_day_harib00i_OS}
  DEPENDS 03_day_harib00i_img
)
add_custom_target(03_day_harib00i_debug
  COMMAND ${QEMU} -s -S ${QEMUOPT} ${03_day_harib00i_OS} ${QEMU_DEBUG_OPT}
  DEPENDS 03_day_harib00i_img
)
add_custom_target(03_day_harib00i_clean
  COMMAND rm -f ${03_day_harib00i_OS}
  COMMAND rm -f ${03_day_harib00i_SYS}
  COMMAND rm -f ${03_day_harib00i_IPLB}
  COMMAND rm -f ${03_day_harib00i_HEADB}
  COMMAND rm -f ${03_day_harib00i_BOOTB}
  COMMAND rm -f ${03_day_harib00i_WILDOBJ}
)
add_custom_target(03_day_harib00i_ipl
  COMMAND ${NASK} ${03_day_harib00i_IPLS} ${03_day_harib00i_IPLB}
)
add_custom_target(03_day_harib00i_sys
  COMMAND ${NASK} ${03_day_harib00i_HEADS} ${03_day_harib00i_HEADB}
  COMMAND gcc ${BINOPT} -T ${03_day_harib00i_LDS} ${03_day_harib00i_CCS} -o ${03_day_harib00i_BOOTB}
  COMMAND cat ${03_day_harib00i_HEADB} ${03_day_harib00i_BOOTB} > ${03_day_harib00i_SYS}
  DEPENDS 03_day_harib00i_ipl
)
add_custom_target(03_day_harib00i_img
  COMMAND mformat -f 1440 -l HARIBOTEOS -N 0xffffffff -C -B ${03_day_harib00i_IPLB} -i ${03_day_harib00i_OS}
  COMMAND mcopy -i ${03_day_harib00i_OS} ${03_day_harib00i_SYS} ::
  DEPENDS 03_day_harib00i_sys
)
########### next target ###############
set(asmhead.img_SRCS ${root_SOURCE_DIR}/projects/03_day/harib00i/asmhead.nas)
set(asmhead.img_OUTS ${root_BINARY_DIR}/projects/03_day/harib00i/asmhead.img)
set(asmhead_wine.img_OUTS ${root_BINARY_DIR}/projects/03_day/harib00i/asmhead_wine.img)

add_custom_target(03_day_harib00i_asmhead
  COMMAND ${root_BINARY_DIR}/src/opennask ${asmhead.img_SRCS} ${asmhead.img_OUTS}
)
add_custom_target(03_day_harib00i_asmhead_wine
  COMMAND ${WINE} ${WINE_NASK} ${asmhead.img_SRCS} ${asmhead_wine.img_OUTS}
)
add_custom_target(03_day_harib00i_asmhead_od
  COMMAND ${OD} -t x1 ${asmhead.img_OUTS}      > ${asmhead.img_OUTS}_f.txt
  COMMAND ${OD} -t x1 ${asmhead_wine.img_OUTS} > ${asmhead_wine.img_OUTS}_t.txt
  COMMAND diff -s ${asmhead.img_OUTS}_f.txt ${asmhead_wine.img_OUTS}_t.txt
)

add_dependencies(03_day_harib00i_asmhead_od 03_day_harib00i_asmhead)
add_dependencies(03_day_harib00i_asmhead_od 03_day_harib00i_asmhead_wine)

add_dependencies(images 03_day_harib00i_asmhead)
add_dependencies(wine 03_day_harib00i_asmhead_wine)
add_dependencies(od 03_day_harib00i_asmhead_od)

#----------------------------------------------------------
########### next target ###############
set(ipl10.img_SRCS ${root_SOURCE_DIR}/projects/03_day/harib00i/ipl10.nas)
set(ipl10.img_OUTS ${root_BINARY_DIR}/projects/03_day/harib00i/ipl10.img)
set(ipl10_wine.img_OUTS ${root_BINARY_DIR}/projects/03_day/harib00i/ipl10_wine.img)

add_custom_target(03_day_harib00i_ipl10
  COMMAND ${root_BINARY_DIR}/src/opennask ${ipl10.img_SRCS} ${ipl10.img_OUTS}
)
add_custom_target(03_day_harib00i_ipl10_wine
  COMMAND ${WINE} ${WINE_NASK} ${ipl10.img_SRCS} ${ipl10_wine.img_OUTS}
)
add_custom_target(03_day_harib00i_ipl10_od
  COMMAND ${OD} -t x1 ${ipl10.img_OUTS}      > ${ipl10.img_OUTS}_f.txt
  COMMAND ${OD} -t x1 ${ipl10_wine.img_OUTS} > ${ipl10_wine.img_OUTS}_t.txt
  COMMAND diff -s ${ipl10.img_OUTS}_f.txt ${ipl10_wine.img_OUTS}_t.txt
)

add_dependencies(03_day_harib00i_ipl10_od 03_day_harib00i_ipl10)
add_dependencies(03_day_harib00i_ipl10_od 03_day_harib00i_ipl10_wine)

add_dependencies(images 03_day_harib00i_ipl10)
add_dependencies(wine 03_day_harib00i_ipl10_wine)
add_dependencies(od 03_day_harib00i_ipl10_od)

#----------------------------------------------------------
