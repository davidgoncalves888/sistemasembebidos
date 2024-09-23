main.elf: main.c startup.c
	arm-none-eabi-gcc -I ./includes -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -c -o startup.o startup.c
	arm-none-eabi-gcc -I ./includes -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -c -o main.o main.c
	arm-none-eabi-gcc -O0 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs \-Wl,--gc-sections,-Map=main.map -Tlink.ld main.o startup.o -o main.elf

flash: main.elf
	openocs -f openocd.cfg -c "program main-elf verify reset exit"

clean:
	rm -rf main.o startup.o

calvo:
	make clean
	rm -rf main.elf