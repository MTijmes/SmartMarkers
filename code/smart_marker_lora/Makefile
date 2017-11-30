# Include platform-specific tools
include tools.mk

# Project name
PROJECT = smart_marker_lora

# List all C defines here
DEFS  = -DSTM32L0
DEFS += -DSTM32L073xx
DEFS += -DUSE_HAL_DRIVER
DEFS += -DREGION_EU868
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

# List of source files
SRC  = ./src/main.c
SRC += ./src/system_stm32l0xx.c

# LoRaMAC source files
SRC += ./lora/mac/LoRaMac.c
SRC += ./lora/mac/LoRaMacCrypto.c
SRC += ./lora/mac/region/Region.c
SRC += ./lora/mac/region/RegionCommon.c
SRC += ./lora/mac/region/RegionEU868.c
SRC += ./lora/radio/sx1272/sx1272.c
SRC += ./lora/system/delay.c
SRC += ./lora/system/fifo.c
SRC += ./lora/system/gpio.c
SRC += ./lora/system/timer.c
SRC += ./lora/system/uart.c
SRC += ./lora/system/crypto/aes.c
SRC += ./lora/system/crypto/cmac.c

# Board source files
SRC += ./drivers/bsp/board.c
SRC += ./drivers/bsp/gpio-board.c
SRC += ./drivers/bsp/rtc-board.c
SRC += ./drivers/bsp/spi-board.c
SRC += ./drivers/bsp/sx1272-board.c
SRC += ./drivers/bsp/uart-board.c
SRC += ./drivers/bsp/utilities.c

# HAL source files
SRC += ./drivers/hal/src/stm32l0xx_hal.c
SRC += ./drivers/hal/src/stm32l0xx_hal_dma.c
SRC += ./drivers/hal/src/stm32l0xx_hal_flash.c
SRC += ./drivers/hal/src/stm32l0xx_hal_gpio.c
SRC += ./drivers/hal/src/stm32l0xx_hal_pwr.c
SRC += ./drivers/hal/src/stm32l0xx_hal_pwr_ex.c
SRC += ./drivers/hal/src/stm32l0xx_hal_rcc.c
SRC += ./drivers/hal/src/stm32l0xx_hal_rcc_ex.c
SRC += ./drivers/hal/src/stm32l0xx_hal_rtc.c
SRC += ./drivers/hal/src/stm32l0xx_hal_rtc_ex.c
SRC += ./drivers/hal/src/stm32l0xx_hal_spi.c
SRC += ./drivers/hal/src/stm32l0xx_hal_uart.c
SRC += ./drivers/hal/src/stm32l0xx_hal_uart_ex.c
SRC += ./drivers/hal/src/stm32l0xx_hal_cortex.c

# List all include directories here
INCDIRS  = ./inc
INCDIRS += ./drivers/bsp
INCDIRS += ./drivers/cmsis/inc
INCDIRS += ./drivers/cmsis/device
INCDIRS += ./drivers/hal/inc

# LoRaMAC includes
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

clean:
	-rm -rf $(OBJS)
	-rm -rf $(PROJECT).elf
	-rm -rf $(PROJECT).map
	-rm -rf $(PROJECT).hex
	-rm -rf $(PROJECT).bin
	-rm -rf $(SRC:.c=.lst)
	-rm -rf $(ASRC:.s=.lst)
	-rm -rf $(STARTUP:.s=.lst)
