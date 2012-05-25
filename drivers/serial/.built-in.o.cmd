cmd_drivers/serial/built-in.o :=  arm-none-eabi-ld -EL    -r -o drivers/serial/built-in.o drivers/serial/serial_core.o drivers/serial/omap-serial.o 
