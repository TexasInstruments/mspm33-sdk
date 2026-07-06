MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = main.obj ti_eeprom_emulation_type_a_eeprom_emulation_type_a.obj ti_eeprom_emulation_type_b_eeprom_emulation_type_b.obj ti_msp_dl_config.obj gcc_startup_mspm33c321x_gcc.obj gcc_system_mspm33c321x.obj

NAME = eeprom_emulation_type_b_test


DFLAGS += -gstrict-dwarf

CFLAGS += -I.. \
    -D__MSPM33C321A__ \
    -O2 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    -mcpu=cortex-m33 \
    -mthumb \
    -std=c99 \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -Wall \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"


ifneq ($(CONFIG),RELEASE)
# Add debug flags by default (excluded only when CONFIG=RELEASE)
CFLAGS += $(DFLAGS)
endif

LFLAGS += "-L$(MSPM33_SDK_INSTALL_DIR)/source/ti/driverlib/lib/gcc/m33/mspm33c321x" \
    -nostartfiles \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    -l:driverlib.a \
    -Wl,-T,../gcc/mspm33c321a.lds \
    "-Wl,-Map,$(NAME).map" \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L.. \
    -mcpu=cortex-m33 \
    -mthumb \
    -static \
    -Wl,--gc-sections \
    -lgcc \
    -lc \
    -lm \
    --specs=nano.specs \
    --specs=nosys.specs

all: $(NAME).out

main.obj: ../main.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

ti_eeprom_emulation_type_a_eeprom_emulation_type_a.obj: ../ti/eeprom/emulation_type_a/eeprom_emulation_type_a.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

ti_eeprom_emulation_type_b_eeprom_emulation_type_b.obj: ../ti/eeprom/emulation_type_b/eeprom_emulation_type_b.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

ti_msp_dl_config.obj: ../ti_msp_dl_config.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

gcc_startup_mspm33c321x_gcc.obj: ../gcc/startup_mspm33c321x_gcc.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

gcc_system_mspm33c321x.obj: ../gcc/system_mspm33c321x.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	$(V) $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	$(V) $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).out > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).map > $(DEVNULL) 2>&1
