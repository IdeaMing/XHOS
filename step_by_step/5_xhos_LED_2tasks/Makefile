# file :Makefile
All:
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c start.s -o start.o
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c user_main.c  -o main.o
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c xhos.c -o xhos.o
	arm-none-eabi-ld -Txhos_stm32.ld start.o main.o xhos.o -o osLED
	arm-none-eabi-objcopy -O binary -S osLED osLED.bin
	arm-none-eabi-objdump -D -m arm  osLED > osLED.dis
clean:
	-rm *.o osLED*
 
flash: clean All
	sudo stm32flash -w osLED.bin -v -g 0 /dev/ttyUSB0
