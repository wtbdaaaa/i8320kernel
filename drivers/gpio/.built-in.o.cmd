cmd_drivers/gpio/built-in.o :=  arm-none-eabi-ld -EL    -r -o drivers/gpio/built-in.o drivers/gpio/gpiolib.o drivers/gpio/twl4030-gpio.o 
