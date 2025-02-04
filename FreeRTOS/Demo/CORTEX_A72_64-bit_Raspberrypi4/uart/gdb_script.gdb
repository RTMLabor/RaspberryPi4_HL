
target extended-remote localhost:3333
set architecture aarch64
set processor armv8-a

load uart.elf

set $pc = _boot

continue
