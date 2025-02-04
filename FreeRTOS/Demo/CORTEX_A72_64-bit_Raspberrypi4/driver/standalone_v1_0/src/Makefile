###############################################################################
#
# Copyright (C) 2011 - 2015 Xilinx, Inc.  All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# Use of the Software is limited solely to applications:
# (a) running on a Xilinx device, or
# (b) that interact with a Xilinx device through a bus or interconnect.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
# OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Except as contained in this notice, the name of the Xilinx shall not be used
# in advertising or otherwise to promote the sale, use or other dealings in
# this Software without prior written authorization from Xilinx.
#
###############################################################################

include config.make

CC=$(COMPILER)-gcc
AS=$(COMPILER)-as
AR=$(ARCHIVER)
CP=cp
COMPILER_FLAGS= -DINIT_MMU=1
EXTRA_COMPILER_FLAGS=
LIB=libbcm.a

CC_FLAGS = $(subst -pg, -DPROFILING, $(COMPILER_FLAGS))
ECC_FLAGS = $(subst -pg, -DPROFILING, $(EXTRA_COMPILER_FLAGS))

ifeq (($(notdir $(CC))) , arm-none-eabi-gcc)
ECC_FLAGS	+= -nostartfiles
endif

RELEASEDIR=../../../lib
INCLUDEDIR=../../../include
INCLUDES=-isystem ./. -isystem ${INCLUDEDIR} -isystem ../../../../../../../Common/FreeRTOS/FreeRTOS/Source/include \
		 -isystem ../../../../../../../Common/FreeRTOS/Core0Application/src \
		 -isystem ../../../../../../../Common/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CA72_64_BIT \
		 
OUTS = *.o
LIBSOURCES := $(wildcard *.c)
ASSEMBLYFILES := $(wildcard *.S)
INCLUDEFILES=*.h

OBJECTS = $(patsubst %.c, %.c.o, $(LIBSOURCES)) $(patsubst %.S, %.S.o, $(ASSEMBLYFILES))


libs: $(LIBS)

%.c.o: %.c
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -c -o $@ $<

%.S.o: %.S
	${CC}.exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) -c -o $@ $<

standalone_libs: $(LIBSOURCES)
	echo "Compiling standalone"
	$(CC).exe $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) $^
	$(AR).exe -r ${RELEASEDIR}/${LIB} ${OUTS}

profile_libs:
	#$(MAKE) -C profile COMPILER_FLAGS="$(COMPILER_FLAGS)" EXTRA_COMPILER_FLAGS="$(EXTRA_COMPILER_FLAGS)" COMPILER="$(CC)" ARCHIVER="$(AR)" libs

.PHONY: include
include: standalone_includes profile_includes

standalone_includes:
	${CP} ${INCLUDEFILES} ${INCLUDEDIR}

profile_includes:
	#$(MAKE) -C profile COMPILER_FLAGS="$(COMPILER_FLAGS)" EXTRA_COMPILER_FLAGS="$(EXTRA_COMPILER_FLAGS)" COMPILER="$(CC)" ARCHIVER="$(AR)" include

clean:
	rm -rf ${OBJECTS}
	rm -rf ${ASSEMBLY_OBJECTS}
	$(MAKE) -C profile COMPILER_FLAGS="$(COMPILER_FLAGS)" EXTRA_COMPILER_FLAGS="$(EXTRA_COMPILER_FLAGS)" COMPILER="$(CC)" ARCHIVER="$(AR)" clean
