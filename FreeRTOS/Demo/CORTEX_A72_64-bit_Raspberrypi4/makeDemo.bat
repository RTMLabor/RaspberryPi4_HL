
SET PATH=%PATH%;C:\msys64\usr\bin;C:\camel-dev\D71\gcc-arm-10.3-2021.07-mingw-w64-i686-aarch64-none-elf\bin

cd uart
make CROSS=aarch64-none-elf- all

pause