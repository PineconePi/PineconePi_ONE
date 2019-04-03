# Select the board to build for: if not given on the command line, then default to SWM320Lite
BOARD ?= SWM320Lite
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)


include ../../py/mkenv.mk


CROSS_COMPILE ?= arm-none-eabi-

CFLAGS_CORTEX_M4 = -mthumb -march=armv7e-m -mabi=aapcs -mcpu=cortex-m4 -msoft-float -mfloat-abi=soft -fsingle-precision-constant -Wdouble-promotion
CFLAGS = -Wall -Wpointer-arith -Werror -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) -O0
CFLAGS += -g -ffunction-sections -fdata-sections -fno-common -fsigned-char -mno-unaligned-access
CFLAGS += $(CFLAGS_MOD)

LDFLAGS = -Wl,-nostdlib -Wl,--gc-sections -Wl,-Map=$@.map


# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(BUILD)/pins_qstr.h

include $(TOP)/py/py.mk


INC =  -I.
INC += -I$(TOP)
INC += -I$(TOP)/lib/cmsis/inc
INC += -Ichip
INC += -Imods
INC += -Imisc
INC += -Iboards/$(BOARD)
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr

CPPDEFINES = -Dgcc


MAIN_SRC_C = \
	main.c \
	mphalport.c \
	mpthreadport.c \

CHIP_SRC_C = $(addprefix chip/,\
	SWM320_adc.c \
	SWM320_can.c \
	SWM320_crc.c \
	SWM320_dma.c \
	SWM320_exti.c \
	SWM320_flash.c \
	SWM320_gpio.c \
	SWM320_i2c.c \
	SWM320_lcd.c \
	SWM320_port.c \
	SWM320_pwm.c \
	SWM320_rtc.c \
	SWM320_sdio.c \
	SWM320_spi.c \
	SWM320_timr.c \
	SWM320_uart.c \
	SWM320_wdt.c \
	system_SWM320.c \
)

CHIP_SRC_S = $(addprefix chip/startup/gcc/,\
	startup_SWM320.s \
)

MODS_SRC_C = $(addprefix mods/,\
	moduos.c \
	modutime.c \
	modmachine.c \
	pybpin.c \
	pybuart.c \
	pybflash.c \
	pybadc.c \
	pybspi.c \
	pybtimer.c \
	pybi2c.c \
	pybpwm.c \
)

MISC_SRC_C = $(addprefix misc/,\
	fifo.c \
	bufhelper.c \
	gccollect.c \
	random.c \
)

MISC_SRC_S = $(addprefix misc/,\
	gchelper.s \
)

LIB_SRC_C = $(addprefix lib/,\
	oofatfs/ff.c \
	oofatfs/option/unicode.c \
	libc/string0.c \
	mp-readline/readline.c \
	timeutils/timeutils.c \
	utils/pyexec.c \
	utils/interrupt_char.c \
	utils/sys_stdio_mphal.c \
)

OBJ = $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(MAIN_SRC_C:.c=.o) $(CHIP_SRC_C:.c=.o) $(CHIP_SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(MODS_SRC_C:.c=.o) $(MISC_SRC_C:.c=.o) $(MISC_SRC_S:.s=.o) $(LIB_SRC_C:.c=.o))
OBJ += $(BUILD)/pins.o

# List of sources for qstr extraction
SRC_QSTR += $(MODS_SRC_C) $(MISC_SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

# Add the linker script
LINKER_SCRIPT = swm320.ld
LDFLAGS += -T $(LINKER_SCRIPT)

# Add the firmware specific CFLAGS
CFLAGS += $(CPPDEFINES) $(INC)



all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(OBJ) $(LINKER_SCRIPT)
	$(ECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $< $@


# Making OBJ use an order-only dependency on the generated pins.h file has the side 
# effect of making the pins.h file before we actually compile any of the objects. 
# The normal dependency generation will deal with the case when pins.h is modified. 
$(OBJ): | $(HEADER_BUILD)/pins.h

# Call make-pins.py to generate both pins.c and pins.h
$(BUILD)/pins.c $(HEADER_BUILD)/pins.h $(BUILD)/pins_qstr.h: chip/SWM320_port.h boards/SWM320_prefix.c | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) boards/make-pins.py --af chip/SWM320_port.h --prefix boards/SWM320_prefix.c --hdr $(HEADER_BUILD)/pins.h --qstr $(BUILD)/pins_qstr.h > $(BUILD)/pins.c

$(BUILD)/pins.o: $(BUILD)/pins.c
	$(call compile_c)


# always include MicroPython make rules
include $(TOP)/py/mkrules.mk
