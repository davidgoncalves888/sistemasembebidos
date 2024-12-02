CC = arm-none-eabi-gcc

TARGET_MAIN = main.elf

COMMON_SRC = startup.c \
             $(wildcard ./drivers/*.c) \
             $(wildcard ./FreeRTOS/*.c)

SRC_MAIN = $(COMMON_SRC) \
          main.c

OBJ_MAIN = $(SRC_MAIN:%.c=%.o)

INCLUDES = -I ./includes -I ./include -I ./FreeRTOS
CPU = -DCPU_MKL46Z256VLL4

CFLAGS = -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus $(INCLUDES) $(CPU) -c
LDFLAGS = -O0 -g3  -Wall -mthumb -mcpu=cortex-m0plus -specs=nosys.specs -Wl,--gc-sections,

OPENOCD_CMD = openocd -f openocd.cfg -c "program main.elf verify reset exit"

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET_MAIN): $(OBJ_MAIN)
	$(CC) $(LDFLAGS)-Map=$(@:.elf=.map) -Tlink.ld $(OBJ_MAIN) -o $(TARGET_MAIN)

flash_main: $(TARGET_MAIN)
	openocd -f openocd.cfg -c "program $< verify reset exit"

clean:
	rm -f $(OBJ_MAIN) $(OBJ_MAIN) $(TARGET_MAIN) $(TARGET_MAIN) $(TARGET_MAIN:.elf=.map)