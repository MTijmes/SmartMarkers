# Platform-specific variables
include tools.mk

# Project name
PROJECT = STM32_LRWAN_112

# List all C defines here
DEFS  = -DSTM32L0 -DSTM32L073xx
DEFS += -DDEBUG
DEFS += -DTRACE
DEFS += -DUSE_STM32L0XX_NUCLEO
DEFS += -DLOW_POWER_DISABLE
DEFS += -DREGION_EU868

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
SRC += ./src/system_syscalls.c
#SRC += ./src/system_stm32l0xx.c
SRC += ./drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c


SRC += ./src/bsp.c
SRC += ./src/debug.c
SRC += ./src/hw_gpio.c
SRC += ./src/hw_rtc.c
SRC += ./src/hw_spi.c
SRC += ./src/stm32l0xx_hal_msp.c
SRC += ./src/stm32l0xx_hw.c
SRC += ./src/stm32l0xx_it.c
SRC += ./src/vcom.c

SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_adc.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_adc_ex.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_cortex.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_gpio.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_pwr.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_pwr_ex.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_rcc.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_rcc_ex.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_rtc.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_rtc_ex.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_spi.c
SRC += ./drivers/STM32L0xx_HAL_Driver/src/stm32l0xx_hal_uart.c

SRC += ./drivers/BSP/STM32L0xx_Nucleo/stm32l0xx_nucleo.c
SRC += ./drivers/BSP/sx1272mb2das/sx1272mb2das.c
SRC += ./drivers/BSP/Components/sx1272/sx1272.c

SRC += ./middlewares/lora/core/lora.c
SRC += ./middlewares/lora/crypto/aes.c
SRC += ./middlewares/lora/crypto/cmac.c
SRC += ./middlewares/lora/Mac/LoRaMac.c
SRC += ./middlewares/lora/Mac/LoRaMacCrypto.c
SRC += ./middlewares/lora/Mac/Region/Region.c
SRC += ./middlewares/lora/Mac/Region/RegionCommon.c
SRC += ./middlewares/lora/Mac/Region/RegionEU868.c
SRC += ./middlewares/lora/Utilities/delay.c
SRC += ./middlewares/lora/Utilities/low_power.c
SRC += ./middlewares/lora/Utilities/timeServer.c
SRC += ./middlewares/lora/Utilities/utilities.c

# List assembly startup source file here
STARTUP = ./startup/startup_stm32l073xx.s

# List all include directories here
INCDIRS  = ./inc
INCDIRS += ./drivers/CMSIS/Include
INCDIRS += ./drivers/CMSIS/Device/ST/STM32L0xx/Include
INCDIRS += ./drivers/STM32L0xx_HAL_Driver/inc
INCDIRS += ./drivers/BSP/STM32L0xx_Nucleo
INCDIRS += ./drivers/BSP/sx1272mb2das
INCDIRS += ./drivers/BSP/Components/sx1272
INCDIRS += ./middlewares/lora/core
INCDIRS += ./middlewares/lora/crypto
INCDIRS += ./middlewares/lora/mac
INCDIRS += ./middlewares/lora/mac/region
INCDIRS += ./middlewares/lora/phy
INCDIRS += ./middlewares/lora/utilities

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
