CC = arm-none-eabi-gcc

TARGET_LED = led_blinky.elf
TARGET_HELLO = hello_world.elf

COMMON_SRC = startup.c \
             $(wildcard ./drivers/*.c)

SRC_LED = $(COMMON_SRC) \
          pin_mux_led.c \
          led_blinky.c

SRC_HELLO = $(COMMON_SRC) \
           pin_mux_hello.c \
           hello_world.c

OBJ_LED = $(SRC_LED:%.c=%.o)
OBJ_HELLO = $(SRC_HELLO:%.c=%.o)

INCLUDES = -I ./includes -I ./drivers
CPU = -DCPU_MKL46Z256VLL4

CFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus $(INCLUDES) $(CPU) -c
LDFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus -specs=nano.specs -Wl,--gc-sections,

OPENOCD_CMD = openocd -f openocd.cfg -c "program main.elf verify reset exit"

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET_LED): $(OBJ_LED)
	$(CC) $(LDFLAGS)-Map=$(@:.elf=.map) -Tlink.ld $(OBJ_LED) -o $(TARGET_LED)

$(TARGET_HELLO): $(OBJ_HELLO)
	$(CC) $(LDFLAGS)-Map=$(@:.elf=.map) -Tlink.ld $(OBJ_HELLO) -o $(TARGET_HELLO)

flash_led: $(TARGET_LED)
	openocd -f openocd.cfg -c "program $< verify reset exit"

flash_hello: $(TARGET_HELLO)
	openocd -f openocd.cfg -c "program $< verify reset exit"

clean:
	rm -f $(OBJ_LED) $(OBJ_HELLO) $(TARGET_LED) $(TARGET_HELLO) $(TARGET_LED:.elf=.map) $(TARGET_HELLO:.elf=.map)
