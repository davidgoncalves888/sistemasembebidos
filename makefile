CC = arm-none-eabi-gcc

# Targets for each compilation
TARGET_EMBEBIDO = emb.elf
TARGET_ENSAMBLAR = ens.elf

# Common source files
COMMON_SRC = startup.c main.c

# Source files and objects for each driver version
SRC_EMBEBIDO = $(COMMON_SRC) $(wildcard ./drivers_EMB/*.c)
SRC_ENSAMBLAR = $(COMMON_SRC) $(wildcard ./drivers_ENS/*.c)

OBJ_EMBEBIDO = $(SRC_EMBEBIDO:%.c=%.o)
OBJ_ENSAMBLAR = $(SRC_ENSAMBLAR:%.c=%.o) ./drivers_ENS/My_Div2.o

# Includes for each driver version
INCLUDES = -I ./includes
INCLUDES_EMB = -I ./includes -I ./drivers_EMB
INCLUDES_ENS = -I ./includes -I ./drivers_ENS

# Common compilation flags
CPU = -DCPU_MKL46Z256VLL4
CFLAGS = -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus $(CPU) -c
LDFLAGS = -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -specs=nano.specs -Wl,--gc-sections,

# Rules for generating .o files for each driver version
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

drivers_EMB/%.o: drivers_EMB/%.c
	$(CC) $(CFLAGS) $(INCLUDES_EMB) -o $@ $<

drivers_ENS/%.o: drivers_ENS/%.c
	$(CC) $(CFLAGS) $(INCLUDES_ENS) -o $@ $<

# Rule to assemble My_Div2.s to My_Div2.o
./drivers_ENS/My_Div2.o: ./drivers_ENS/My_Div2.s
	arm-none-eabi-as -mthumb -mcpu=cortex-m0plus -o $@ $<

# Linking rules for each target
$(TARGET_EMBEBIDO): $(OBJ_EMBEBIDO)
	$(CC) $(LDFLAGS)-Map=$(@:.elf=.map) -Tlink.ld $(OBJ_EMBEBIDO) -o $(TARGET_EMBEBIDO)

$(TARGET_ENSAMBLAR): $(OBJ_ENSAMBLAR)
	$(CC) $(LDFLAGS)-Map=$(@:.elf=.map) -Tlink.ld $(OBJ_ENSAMBLAR) -o $(TARGET_ENSAMBLAR)

# Flash commands for each target
flash_embebido: $(TARGET_EMBEBIDO)
	openocd -f openocd.cfg -c "program $< verify reset exit"

flash_ensamblar: $(TARGET_ENSAMBLAR)
	openocd -f openocd.cfg -c "program $< verify reset exit"

# Clean up objects and targets
clean:
	rm -f $(OBJ_EMBEBIDO) $(OBJ_ENSAMBLAR) $(TARGET_EMBEBIDO) $(TARGET_ENSAMBLAR) $(TARGET_EMBEBIDO:.elf=.map) $(TARGET_ENSAMBLAR:.elf=.map)