CC = arm-none-eabi-gcc

TARGET_MAIN = main.elf

COMMON_SRC = startup.c \
             $(wildcard ./drivers/*.c)

SRC_MAIN = $(COMMON_SRC) \
          main.c

OBJ_MAIN = $(SRC_MAIN:%.c=%.o)

INCLUDES = -I ./includes -I ./drivers
CPU = -DCPU_MKL46Z256VLL4

CFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus $(INCLUDES) $(CPU) -c
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus -specs=nano.specs -Wl,--gc-sections,

OPENOCD_CMD = openocd -f openocd.cfg -c "program main.elf verify reset exit"

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET_MAIN): $(OBJ_MAIN)
	$(CC) $(LDFLAGS)-Map=$(@:.elf=.map) -Tlink.ld $(OBJ_MAIN) -o $(TARGET_MAIN)

flash_main: $(TARGET_MAIN)
	openocd -f openocd.cfg -c "program $< verify reset exit"

clean:
	rm -f $(OBJ_LED) $(OBJ_HELLO) $(TARGET_LED) $(TARGET_HELLO) $(TARGET_LED:.elf=.map) $(TARGET_HELLO:.elf=.map)