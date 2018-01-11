# Include platform-specific tools
include tools.mk

# Project name
PROJECT = smart_marker

# List all C defines here
DEFS  = -DSTM32L0
DEFS += -DSTM32L073xx
DEFS += -DUSE_FULL_LL_DRIVER
DEFS += -DREGION_EU868
#DEFS += -DDEBUG

# Debug level
DEBUG = -g3
#DEBUG = -g0

# Define optimisation level here
# OPT = -Os
OPT = -O0

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

# List of source files
SRC = ./src/syscalls.c
SRC += ./src/main.c
SRC += ./src/stm32l0xx_it.c
SRC += ./src/system_stm32l0xx.c
SRC += ./src/ublox.c

# BSP source files
SRC += ./drivers/bsp/smart_marker/board.c
SRC += ./drivers/bsp/smart_marker/gps.c
SRC += ./drivers/bsp/smart_marker/i2c.c
SRC += ./drivers/bsp/smart_marker/lora.c
SRC += ./drivers/bsp/smart_marker/rtc.c
SRC += ./drivers/bsp/smart_marker/spi.c
SRC += ./drivers/bsp/smart_marker/sx1272-board.c
SRC += ./drivers/bsp/smart_marker/uart.c

SRC += ./drivers/bsp/smart_marker/utilities.c

# LL source files
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_rcc.c
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_utils.c
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_gpio.c
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_exti.c
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_usart.c
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_pwr.c
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_i2c.c
SRC += ./drivers/stm32l0xx_hal/src/stm32l0xx_ll_rtc.c

# LoRaMac source files
SRC += ./lora/mac/LoRaMac.c
SRC += ./lora/mac/LoRaMacCrypto.c
SRC += ./lora/mac/region/Region.c
SRC += ./lora/mac/region/RegionCommon.c
SRC += ./lora/mac/region/RegionEU868.c
SRC += ./lora/radio/sx1272/sx1272.c
SRC += ./lora/system/timer.c
SRC += ./lora/system/crypto/aes.c
SRC += ./lora/system/crypto/cmac.c

# List all include directories here
INCDIRS  = ./inc
INCDIRS += ./drivers/bsp/smart_marker
INCDIRS += ./drivers/cmsis/include
INCDIRS += ./drivers/cmsis/device/st/stm32l0xx/include
INCDIRS += ./drivers/stm32l0xx_hal/inc

# LoRaMac includes
INCDIRS += ./lora/mac
INCDIRS += ./lora/mac/region
INCDIRS += ./lora/radio
INCDIRS += ./lora/radio/sx1272
INCDIRS += ./lora/system
INCDIRS += ./lora/system/crypto

# List the user directory to look for the libraries here
LIBDIRS +=

# List all user libraries here
LIBS =

# List assembly startup source file here
STARTUP = ./startup/startup_stm32l073xx.s

# Define linker script file here
LINKER_SCRIPT = ./linker/stm32l073xx_flash.ld

# Dirs
OBJDIR  = obj
OBJS    = $(STARTUP:.s=.o) $(SRC:.c=.o)
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

server:
	$(STLINK)/st-util

clean:
	-rm -rf $(OBJS)
	-rm -rf $(PROJECT).elf
	-rm -rf $(PROJECT).map
	-rm -rf $(PROJECT).hex
	-rm -rf $(PROJECT).bin
	-rm -rf $(SRC:.c=.lst)
	-rm -rf $(ASRC:.s=.lst)
	-rm -rf $(STARTUP:.s=.lst)
