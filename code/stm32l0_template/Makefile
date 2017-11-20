# Project name
PROJECT = MyProject

# List all C defines here
DEFS = -DSTM32L0 -DSTM32L073xx
#DEFS += -DDEBUG

# Debug level
DEBUG = -g3
#DEBUG = -g0

# Define optimisation level here
OPT = -Os

# MCU type
MCU  = cortex-m0plus

# Tools
PREFIX = arm-none-eabi-
CC   = $(PREFIX)gcc
CXX  = $(PREFIX)g++
GDB  = $(PREFIX)gdb
CP   = $(PREFIX)objcopy
AS   = $(PREFIX)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S

#Path to Texane's stlink tools
STLINK = /home/lskalski/Programs/stlink

# List of source files
SRC  = ./src/main.c
SRC += ./src/system_syscalls.c
SRC += ./src/system_stm32l0xx.c

# List assembly startup source file here
STARTUP = ./startup/startup_stm32l073xx.s
 
# List all include directories here
INCDIRS = ./inc ./inc/CMSIS/core ./inc/CMSIS/device
              
# List the user directory to look for the libraries here
LIBDIRS += 
 
# List all user libraries here
LIBS =
 
# Define linker script file here
LINKER_SCRIPT = ./linker/linker_stm32l073xx.ld

# Dirs
OBJS  = $(STARTUP:.s=.o) $(SRC:.c=.o)
INCDIR  = $(patsubst %,-I%, $(INCDIRS))
LIBDIR  = $(patsubst %,-L%, $(LIBDIRS))
LIB     = $(patsubst %,-l%, $(LIBS))

# Flags
COMMONFLAGS = -mcpu=$(MCU) -mthumb -mfloat-abi=soft
ASFLAGS = $(COMMONFLAGS) $(DEBUG)
CPFLAGS = $(COMMONFLAGS) $(OPT) $(DEFS) $(DEBUG) -Wall -fmessage-length=0 -ffunction-sections
LDFLAGS = $(COMMONFLAGS) -T$(LINKER_SCRIPT) -Wl,-Map=$(PROJECT).map -Wl,--gc-sections $(LIBDIR) $(LIB)
 
#
# Makefile Rules
#
 
all: $(OBJS) $(PROJECT).elf  $(PROJECT).hex $(PROJECT).bin
	$(TRGT)size $(PROJECT).elf
 
%.o: %.c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%.elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%.hex: %.elf
	$(HEX) $< $@

%.bin: %.elf
	$(BIN) $< $@

flash: $(PROJECT).bin
	$(STLINK)/st-flash write $(PROJECT).bin 0x8000000

erase:
	$(STLINK)/st-flash erase

debug: $(PROJECT).elf
	$(GDB) --eval-command="target remote localhost:4242" $(PROJECT).elf -ex 'load'

clean:
	-rm -rf $(OBJS)
	-rm -rf $(PROJECT).elf
	-rm -rf $(PROJECT).map
	-rm -rf $(PROJECT).hex
	-rm -rf $(PROJECT).bin
	-rm -rf $(SRC:.c=.lst)
	-rm -rf $(ASRC:.s=.lst)
	-rm -rf $(STARTUP:.s=.lst)
