REM 1
start /b putty -load "RPi4_Ubuntu"

REM 1.1
start /b putty -load "RPi4_FreeRTOS"


REM 2 hook up with openocd
start cmd /b /k "openocd -f .\..\..\..\..\DebugProcess\OpenOCD_Debugging\OpenOCD-20240820-0.12.0\bin\interface\ft232h.cfg -f .\..\..\..\..\DebugProcess\OpenOCD_Debugging\OpenOCD-20240820-0.12.0\bin\target\rpi4b.cfg -d3"

REM 3 connect via telnet
start cmd /b /k "telnet 127.0.0.1 4444"

REM 4

REM C:\msys64\msys2_shell.cmd -defterm -here -no-start -msys

REM export PATH=$PATH:/c/msys64/usr/bin;C:/Users/ehlingn/SAFE_Ehling/Projekte/RPi/TestRig-Interfaces/SPI-Ethernet/LLD/LLD_tutorials/gcc-arm-11.2-2022.02-mingw-w64-i686-aarch64-none-linux-gnu/bin

REM gdb-multiarch uart.elf
