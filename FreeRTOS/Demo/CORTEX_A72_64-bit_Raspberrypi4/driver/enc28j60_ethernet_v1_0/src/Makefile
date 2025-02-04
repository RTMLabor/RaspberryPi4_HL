CC=$(COMPILER)-gcc
AR=$(ARCHIVER)
ARCHIVER=
CP=cp
COMPILER_FLAGS=
EXTRA_COMPILER_FLAGS=
LIB=libbcm.a

CC_FLAGS = $(COMPILER_FLAGS)
ECC_FLAGS = $(EXTRA_COMPILER_FLAGS)

RELEASEDIR=../../../lib
INCLUDEDIR=../../../include
INCLUDES=-isystem ./. -isystem ${INCLUDEDIR} -isystem ../../../../../../../Common/FreeRTOS/FreeRTOS/Source/include \
		 -isystem ../../../../../../../Common/FreeRTOS/Core0Application/src \
		 -isystem ../../../../../../../Common/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CA72_64_BIT

OUTS = *.o

LIBSOURCES:=*.c
ASSEMBLYFILES:=*.S
INCLUDEFILES:=*.h

OBJECTS = $(addsuffix .o, $(basename $(wildcard $(LIBSOURCES)) $(wildcard $(ASSEMBLYFILES))))


libs: banner uart_rpi4_libs clean

%.o: %.c
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -o $@ $<

%.o: %.S
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -o $@ $<

banner:
	echo "Compiling mini_uart"

uart_rpi4_libs: ${OBJECTS}
	$(AR).exe -r ${RELEASEDIR}/${LIB} ${OBJECTS}

.PHONY: include
include: uart_rpi4_includes

uart_rpi4_includes:
	${CP} ${INCLUDEFILES} ${INCLUDEDIR}

clean:
	rm -rf ${OBJECTS}
