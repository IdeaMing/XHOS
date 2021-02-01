# file :Makefile

All:
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c start.S -o start.o
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c main.c -o main.o
	arm-none-eabi-ld  -Ttext=0x08000000 start.o main.o -o LED
	arm-none-eabi-objcopy -O binary -S LED LED.bin
	arm-none-eabi-objdump -D -m arm  LED > LED.dis
clean:
	-rm *.o LED*
 
flash: 
	sudo stm32flash -w LED.bin -v -g 0 /dev/ttyUSB0
