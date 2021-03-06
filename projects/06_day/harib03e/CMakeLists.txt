#----------------------------------------------------------
message(STATUS "Entering directory projects/06_day/harib03e/")

set(NASK ${root_BINARY_DIR}/src/opennask)
set(FONT ${root_BINARY_DIR}/src/makefont)
set(B2O  ${root_BINARY_DIR}/src/bin2obj)
set(CONV ${root_BINARY_DIR}/objconv/objconv)
set(06_day_harib03e_OS    ${root_BINARY_DIR}/projects/06_day/harib03e/os.img)
set(06_day_harib03e_SYS	  ${root_BINARY_DIR}/projects/06_day/harib03e/os.sys)
set(06_day_harib03e_IPLB  ${root_BINARY_DIR}/projects/06_day/harib03e/ipl.bin)
set(06_day_harib03e_IPLS  ${root_SOURCE_DIR}/projects/06_day/harib03e/ipl10.nas)
set(06_day_harib03e_HEADB ${root_BINARY_DIR}/projects/06_day/harib03e/asmhead.bin)
set(06_day_harib03e_HEADS ${root_SOURCE_DIR}/projects/06_day/harib03e/asmhead.nas)
set(06_day_harib03e_BOOTB ${root_BINARY_DIR}/projects/06_day/harib03e/boot.bin)
set(06_day_harib03e_CCO	  ${root_BINARY_DIR}/projects/06_day/harib03e/boot.o)
set(06_day_harib03e_CCS	  ${root_SOURCE_DIR}/projects/06_day/harib03e/*.c)
set(06_day_harib03e_LDS	  ${root_SOURCE_DIR}/projects/os.lds)
set(06_day_harib03e_FUNCS ${root_SOURCE_DIR}/projects/06_day/harib03e/naskfunc.nas)
set(06_day_harib03e_FUNCO ${root_BINARY_DIR}/projects/06_day/harib03e/naskfunc.o)
set(06_day_harib03e_FONTS ${root_SOURCE_DIR}/projects/06_day/harib03e/hankaku.txt)
set(06_day_harib03e_FONTB ${root_BINARY_DIR}/projects/06_day/harib03e/hankaku.bin)
set(06_day_harib03e_FONTO ${root_BINARY_DIR}/projects/06_day/harib03e/hankaku.o)
set(06_day_harib03e_LIBGE ${root_BINARY_DIR}/golibc/libgo.a)
set(06_day_harib03e_LIBGC ${root_BINARY_DIR}/projects/06_day/harib03e/libgo.o)
set(06_day_harib03e_WILDOBJ ${root_BINARY_DIR}/projects/06_day/harib03e/*.o)


add_custom_target(06_day_harib03e_run
  COMMAND ${QEMU} ${QEMUOPT} ${06_day_harib03e_OS}
  DEPENDS 06_day_harib03e_img
)
add_custom_target(06_day_harib03e_debug
  COMMAND ${QEMU} -s -S ${QEMUOPT} ${06_day_harib03e_OS} ${QEMU_DEBUG_OPT}
  DEPENDS 06_day_harib03e_img
)
add_custom_target(06_day_harib03e_clean
  COMMAND rm -f ${06_day_harib03e_OS}
  COMMAND rm -f ${06_day_harib03e_SYS}
  COMMAND rm -f ${06_day_harib03e_IPLB}
  COMMAND rm -f ${06_day_harib03e_HEADB}
  COMMAND rm -f ${06_day_harib03e_BOOTB}
  COMMAND rm -f ${06_day_harib03e_WILDOBJ}
)
add_custom_target(06_day_harib03e_ipl
  COMMAND ${NASK} ${06_day_harib03e_IPLS} ${06_day_harib03e_IPLB}
)
add_custom_target(06_day_harib03e_sys
  COMMAND ${NASK} ${06_day_harib03e_HEADS} ${06_day_harib03e_HEADB}
  COMMAND ${FONT} ${06_day_harib03e_FONTS} ${06_day_harib03e_FONTB}
  COMMAND ${B2O}  ${06_day_harib03e_FONTB} ${06_day_harib03e_FONTO} _hankaku
  COMMAND ${CONV} -fcoff32 -nu ${06_day_harib03e_LIBGE} ${06_day_harib03e_LIBGC}
  COMMAND ${NASK} ${06_day_harib03e_FUNCS} ${06_day_harib03e_FUNCO}
  COMMAND gcc ${BINOPT} -T ${06_day_harib03e_LDS} ${06_day_harib03e_CCS} ${06_day_harib03e_WILDOBJ} -o ${06_day_harib03e_BOOTB}
  COMMAND cat ${06_day_harib03e_HEADB} ${06_day_harib03e_BOOTB} > ${06_day_harib03e_SYS}
  DEPENDS 06_day_harib03e_ipl
)
add_custom_target(06_day_harib03e_img
  COMMAND mformat -f 1440 -l HARIBOTEOS -N 0xffffffff -C -B ${06_day_harib03e_IPLB} -i ${06_day_harib03e_OS}
  COMMAND mcopy -i ${06_day_harib03e_OS} ${06_day_harib03e_SYS} ::
  DEPENDS 06_day_harib03e_sys
)
########### next target ###############
set(asmhead.img_SRCS ${root_SOURCE_DIR}/projects/06_day/harib03e/asmhead.nas)
set(asmhead.img_OUTS ${root_BINARY_DIR}/projects/06_day/harib03e/asmhead.img)
set(asmhead_wine.img_OUTS ${root_BINARY_DIR}/projects/06_day/harib03e/asmhead_wine.img)

add_custom_target(06_day_harib03e_asmhead
  COMMAND ${root_BINARY_DIR}/src/opennask ${asmhead.img_SRCS} ${asmhead.img_OUTS}
)
add_custom_target(06_day_harib03e_asmhead_wine
  COMMAND ${WINE} ${WINE_NASK} ${asmhead.img_SRCS} ${asmhead_wine.img_OUTS}
)
add_custom_target(06_day_harib03e_asmhead_od
  COMMAND ${OD} -t x1 ${asmhead.img_OUTS}      > ${asmhead.img_OUTS}_f.txt
  COMMAND ${OD} -t x1 ${asmhead_wine.img_OUTS} > ${asmhead_wine.img_OUTS}_t.txt
  COMMAND diff -s ${asmhead.img_OUTS}_f.txt ${asmhead_wine.img_OUTS}_t.txt
)

add_dependencies(06_day_harib03e_asmhead_od 06_day_harib03e_asmhead)
add_dependencies(06_day_harib03e_asmhead_od 06_day_harib03e_asmhead_wine)

add_dependencies(images 06_day_harib03e_asmhead)
add_dependencies(wine 06_day_harib03e_asmhead_wine)
add_dependencies(od 06_day_harib03e_asmhead_od)

#----------------------------------------------------------
########### next target ###############
set(ipl10.img_SRCS ${root_SOURCE_DIR}/projects/06_day/harib03e/ipl10.nas)
set(ipl10.img_OUTS ${root_BINARY_DIR}/projects/06_day/harib03e/ipl10.img)
set(ipl10_wine.img_OUTS ${root_BINARY_DIR}/projects/06_day/harib03e/ipl10_wine.img)

add_custom_target(06_day_harib03e_ipl10
  COMMAND ${root_BINARY_DIR}/src/opennask ${ipl10.img_SRCS} ${ipl10.img_OUTS}
)
add_custom_target(06_day_harib03e_ipl10_wine
  COMMAND ${WINE} ${WINE_NASK} ${ipl10.img_SRCS} ${ipl10_wine.img_OUTS}
)
add_custom_target(06_day_harib03e_ipl10_od
  COMMAND ${OD} -t x1 ${ipl10.img_OUTS}      > ${ipl10.img_OUTS}_f.txt
  COMMAND ${OD} -t x1 ${ipl10_wine.img_OUTS} > ${ipl10_wine.img_OUTS}_t.txt
  COMMAND diff -s ${ipl10.img_OUTS}_f.txt ${ipl10_wine.img_OUTS}_t.txt
)

add_dependencies(06_day_harib03e_ipl10_od 06_day_harib03e_ipl10)
add_dependencies(06_day_harib03e_ipl10_od 06_day_harib03e_ipl10_wine)

add_dependencies(images 06_day_harib03e_ipl10)
add_dependencies(wine 06_day_harib03e_ipl10_wine)
add_dependencies(od 06_day_harib03e_ipl10_od)

#----------------------------------------------------------
########### next target ###############
set(naskfunc.img_SRCS ${root_SOURCE_DIR}/projects/06_day/harib03e/naskfunc.nas)
set(naskfunc.img_OUTS ${root_BINARY_DIR}/projects/06_day/harib03e/naskfunc.img)
set(naskfunc_wine.img_OUTS ${root_BINARY_DIR}/projects/06_day/harib03e/naskfunc_wine.img)

add_custom_target(06_day_harib03e_naskfunc
  COMMAND ${root_BINARY_DIR}/src/opennask ${naskfunc.img_SRCS} ${naskfunc.img_OUTS}
)
add_custom_target(06_day_harib03e_naskfunc_wine
  COMMAND ${WINE} ${WINE_NASK} ${naskfunc.img_SRCS} ${naskfunc_wine.img_OUTS}
)
add_custom_target(06_day_harib03e_naskfunc_od
  COMMAND ${OD} -t x1 ${naskfunc.img_OUTS}      > ${naskfunc.img_OUTS}_f.txt
  COMMAND ${OD} -t x1 ${naskfunc_wine.img_OUTS} > ${naskfunc_wine.img_OUTS}_t.txt
  COMMAND diff -s ${naskfunc.img_OUTS}_f.txt ${naskfunc_wine.img_OUTS}_t.txt
)

add_dependencies(06_day_harib03e_naskfunc_od 06_day_harib03e_naskfunc)
add_dependencies(06_day_harib03e_naskfunc_od 06_day_harib03e_naskfunc_wine)

add_dependencies(images 06_day_harib03e_naskfunc)
add_dependencies(wine 06_day_harib03e_naskfunc_wine)
add_dependencies(od 06_day_harib03e_naskfunc_od)

#----------------------------------------------------------
