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

LIBSOURCES := $(wildcard *.c)
ASSEMBLYFILES := $(wildcard *.S)
INCLUDEFILES:=*.h

#OBJECTS = $(addsuffix .o, $(basename $(wildcard $(LIBSOURCES)) $(wildcard $(ASSEMBLYFILES))))
OBJECTS = $(patsubst %.c, %.c.o, $(LIBSOURCES)) $(patsubst %.S, %.S.o, $(ASSEMBLYFILES))


libs: banner gic_libs clean

%.c.o: %.c
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -c -o $@ $<

%.S.o: %.S
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -c -o $@ $<

banner:
	echo "Compiling gic"

gic_libs: ${OBJECTS}
	$(AR).exe -rcs ${RELEASEDIR}/${LIB} ${OBJECTS}

.PHONY: include
include: gic_includes

gic_includes:
	${CP} ${INCLUDEFILES} ${INCLUDEDIR}

clean:
	rm -rf ${OBJECTS}