REM 1
start /b putty -load "RPi4_Ubuntu"

REM 2 hook up with openocd
start cmd /b /k "openocd -f C:\Users\ehlingn\SAFE_Ehling\Projekte\RPi\CBS-System\Toolboxes\iXnitionTargetRPi4\RaspberryPi4\DebugProcess\OpenOCD_Debugging\OpenOCD-20240820-0.12.0\bin\interface\ft232h.cfg -f C:\Users\ehlingn\SAFE_Ehling\Projekte\RPi\CBS-System\Toolboxes\iXnitionTargetRPi4\RaspberryPi4\DebugProcess\OpenOCD_Debugging\OpenOCD-20240820-0.12.0\bin\target\rpi4b.cfg -d3"

REM 3 connect via telnet
start cmd /b /k "telnet 127.0.0.1 4444 & mdw 0x20000000"

REM 4 gdb connection for deeper debug
gdb-multiarch NetCore.elf
file C:/Users/ehlingn/SAFE_Ehling/Projekte/RPi/CBS-System/Toolboxes/iXnitionTargetRPi4/RaspberryPi4/DebugProcess/ELF-Files/NetCore.elf

target extended-remote localhost:3333
set architecture aarch64

load NetCore.elf 0x30000000
break *_boot
continue

REM 5 check code
cd C:\Users\ehlingn\SAFE_Ehling\Projekte\RPi\CBS-System\Toolboxes\iXnitionTargetRPi4\RaspberryPi4\SetupFiles\Core0\debug
C:\msys64\msys2_shell.cmd -defterm -here -no-start -msys
export PATH=/c/camel-dev/D71/arm-rpi4-gcc-aarch64/bin:$PATH
aarch64-elf-objdump -d NetCore.elf | grep -A 20 "0x20000000"

