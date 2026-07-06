MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = main.obj ti_eeprom_emulation_type_a_eeprom_emulation_type_a.obj ti_eeprom_emulation_type_b_eeprom_emulation_type_b.obj ti_msp_dl_config.obj ticlang_startup_mspm33c321x_ticlang.obj

NAME = eeprom_emulation_type_a


DFLAGS += -gdwarf-3

CFLAGS += -I.. \
    -D__MSPM33C321A__ \
    -O2 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    -mcpu=cortex-m33 \
    -Wall


ifneq ($(CONFIG),RELEASE)
# Add debug flags by default (excluded only when CONFIG=RELEASE)
CFLAGS += $(DFLAGS)
endif

LFLAGS += "-l$(MSPM33_SDK_INSTALL_DIR)/source/ti/driverlib/lib/ticlang/m33/mspm33c321x/driverlib.a" \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L.. \
    ../ticlang/mspm33c321a.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

main.obj: ../main.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_eeprom_emulation_type_a_eeprom_emulation_type_a.obj: ../ti/eeprom/emulation_type_a/eeprom_emulation_type_a.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_eeprom_emulation_type_b_eeprom_emulation_type_b.obj: ../ti/eeprom/emulation_type_b/eeprom_emulation_type_b.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_msp_dl_config.obj: ../ti_msp_dl_config.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ticlang_startup_mspm33c321x_ticlang.obj: ../ticlang/startup_mspm33c321x_ticlang.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	$(V) $(LNK) -Wl,-u,_c_int00 $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	$(V) $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).out > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).map > $(DEVNULL) 2>&1
