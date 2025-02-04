
export "PATH=$PATH:/c/camel-dev/D71/arm-rpi4-gcc-aarch64/bin"

for file in *.c; do
    echo "Dependencies for $file:"
    gcc -MM \
		-I /c/Users/ehlingn/SAFE_Ehling/Projekte/RPi/CBS-System/Toolboxes/iXnitionTargetRPi4_TImada_base/Common/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include \
		-I /c/Users/ehlingn/SAFE_Ehling/Projekte/RPi/CBS-System/Toolboxes/iXnitionTargetRPi4_TImada_base/Common/FreeRTOS/FreeRTOS/Source/include \
		-I /c/Users/ehlingn/SAFE_Ehling/Projekte/RPi/CBS-System/Toolboxes/iXnitionTargetRPi4_TImada_base/Common/FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CA72_64_BIT \
		-I C:/Users/ehlingn/SAFE_Ehling/Projekte/RPi/CBS-System/Toolboxes/iXnitionTargetRPi4_TImada_base/Common/FreeRTOS/FreeRTOS-Plus/Source/FreeRTOS-Plus-UDP/portable/Compiler/GCC \
		-I /c/Users/ehlingn/SAFE_Ehling/Projekte/RPi/CBS-System/Toolboxes/iXnitionTargetRPi4_TImada_base/Common/FreeRTOS/Core0Application/src/ \
		-I /c/Users/ehlingn/SAFE_Ehling/Projekte/RPi/CBS-System/Toolboxes/iXnitionTargetRPi4_TImada_base/RaspberryPi4/BSP/Core0_BSP/bcm2711_cortexa72_0/include \
		"$file"
done