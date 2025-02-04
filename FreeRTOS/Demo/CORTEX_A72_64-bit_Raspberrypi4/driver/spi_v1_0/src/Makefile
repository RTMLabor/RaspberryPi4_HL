CC=$(COMPILER)-gcc
AR=$(ARCHIVER)
CP=cp
COMPILER_FLAGS=
EXTRA_COMPILER_FLAGS=
LIB=libbcm.a

CC_FLAGS = $(COMPILER_FLAGS)
ECC_FLAGS = $(EXTRA_COMPILER_FLAGS)

RELEASEDIR=../../../lib
INCLUDEDIR=../../../include
INCLUDES=-isystem ./. -isystem ${INCLUDEDIR}

OUTS = *.o

LIBSOURCES:=*.c
ASSEMBLYFILES:=*.S
INCLUDEFILES:=*.h

OBJECTS = $(addsuffix .o, $(basename $(wildcard $(LIBSOURCES)) $(wildcard $(ASSEMBLYFILES))))

libs: banner bgpio_libs clean

%.o: %.c
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -o $@ $<

%.o: %.S
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -o $@ $<

banner:
	echo "Compiling bgpio"

bgpio_libs: ${OBJECTS}
	$(AR).exe -r ${RELEASEDIR}/${LIB} ${OBJECTS}

.PHONY: include
include: bgpio_includes

bgpio_includes:
	${CP} ${INCLUDEFILES} ${INCLUDEDIR}

clean:
	rm -rf ${OBJECTS}
