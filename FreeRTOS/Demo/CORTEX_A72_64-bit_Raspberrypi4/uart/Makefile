CROSS ?= aarch64-none-elf-
CFLAGS = -mcpu=cortex-a72 \
         -fpic \
         -ffreestanding \
         -std=gnu99 \
         -O2 \
         -Wall \
         -Wextra \
         -DGUEST \
         $(INCLUDES) \
		 -g3
BUILTIN_OPS = -fno-builtin-memset
ASMFLAGS = -mcpu=cortex-a72

INCLUDE_DIRS = ./src \
			   ../musl_libc \
			   ../mmu \
			   ../../../Source/include \
			   ../../../Source/portable/GCC/ARM_CA72_64_BIT \
			   ../../../../FreeRTOS-Plus/Source/Reliance-Edge/include \
			   ../../../../FreeRTOS-Plus/Source/Reliance-Edge/tests/posix \
			   ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP \
			   ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include \
			   ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/protocols/include \
			   ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-UDP/portable/Compiler/GCC \
			   ../driver/cpu_cortexa72_v1_0/src \
			   ../driver/standalone_v1_0/src \
			   ../driver/enc28j60_ethernet_v1_0/src \
			   ../driver/timer_v1_0/src \
			   ../driver/uart_v1_0/src \
			   ../driver/gic_v1_0/src \
			   ../driver/spi_v1_0/src \
			   ../driver/bgpio_v1_0/src

INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

# From ../drivers/..
OBJS = build/gpio.o \
	   build/utils.o \
	   build/spi0.o \
	   build/enc28j60.o \
	   build/encspi.o \
	   build/printf.o \
	   build/timer.o \
	   build/bcm_mem.o \
	   build/bcm_string.o \
	   build/bcm_io.o

# From ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/..
OBJS +=build/FreeRTOS_ARP.o \
	   build/FreeRTOS_DHCP.o \
	   build/FreeRTOS_DNS.o \
	   build/FreeRTOS_IP.o \
	   build/FreeRTOS_Sockets.o \
	   build/FreeRTOS_Stream_Buffer.o \
	   build/FreeRTOS_TCP_IP.o \
	   build/FreeRTOS_TCP_WIN.o \
	   build/FreeRTOS_UDP_IP.o

# From ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/NetworkInterface/RPi4..
OBJS +=build/NetworkInterface.o

# From ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/BufferManagement..
OBJS +=build/BufferAllocation_1.o

# From ./src
OBJS +=build/startup.o  \
	   build/FreeRTOS_asm_vector.o \
	   build/FreeRTOS_tick_config.o \
	   build/interrupt.o \
	   build/main.o \
	   build/mmu_cfg.o \
	   build/uart.o

# From ../mmu
OBJS +=build/mmu.o

# From ../cache
OBJS +=build/cache.o

# From ../musl_libc
OBJS +=build/memset.o \
	   build/memcpy.o \
   	   build/memmove.o \
	   build/strlen.o \
	   build/strncmp.o \
	   build/strcpy.o \
	   build/strncpy.o \
	   build/snprintf.o \
	   build/strcasecmp.o

# From ../../../Source/portable/GCC/ARM_CA72_64_BIT
OBJS +=build/port.o \
	   build/portASM.o \
	   build/list.o \
	   build/queue.o \
	   build/tasks.o \
	   build/timers.o \
	   build/event_groups.o \
	   build/heap_1.o

BUILDDIR =./build

.PHONY: all clean

all : clean builddir uart.elf

builddir : | $(BUILDDIR)

$(BUILDDIR) :
	mkdir $@

clean :
	rm -rf ./build
	rm -f *.elf
	rm -f *.list

uart.elf : src/raspberrypi4.ld $(OBJS)
	$(CROSS)gcc -Wl,--build-id=none -std=gnu99 -T src/raspberrypi4.ld -o $@ -ffreestanding -nostdlib --specs=nosys.specs $(BUILTIN_OPS) $(OBJS)
	$(CROSS)objdump -d uart.elf > uart.list
	$(CROSS)objcopy -O binary uart.elf kernel8.img
	
build/%.o : ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/NetworkInterface/RPi4/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<
	
build/%.o : ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/BufferManagement/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../../../../FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../driver/timer_v1_0/src/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<
	
build/%.o : ../driver/timer_v1_0/src/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../driver/standalone_v1_0/src/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<
	
build/%.o : ../driver/standalone_v1_0/src/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../driver/enc28j60_ethernet_v1_0/src/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<
	
build/%.o : ../driver/enc28j60_ethernet_v1_0/src/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../driver/spi0_v1_0/src/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<
	
build/%.o : ../driver/spi_v1_0/src/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../driver/uart_v1_0/src/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<
	
build/%.o : ../driver/uart_v1_0/src/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../driver/bgpio_v1_0/src/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<
	
build/%.o : ../driver/bgpio_v1_0/src/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : src/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<
	
build/%.o : src/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../mmu/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../cache/%.S
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../musl_libc/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../../../Source/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../../../Source/portable/GCC/ARM_CA72_64_BIT/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<

build/%.o : ../../../Source/portable/GCC/ARM_CA72_64_BIT/%.S
	$(CROSS)as $(ASMFLAGS) -c -o $@ $<

build/%.o : ../../../Source/portable/MemMang/%.c
	$(CROSS)gcc $(CFLAGS)  -c -o $@ $<


