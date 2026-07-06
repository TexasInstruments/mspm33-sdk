MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

ASM = "$(IAR_ARMCOMPILER)/bin/iasmarm"
CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = main.obj ti_eeprom_emulation_type_a_eeprom_emulation_type_a.obj ti_eeprom_emulation_type_b_eeprom_emulation_type_b.obj ti_msp_dl_config.obj iar_startup_mspm33c321x_iar.obj

NAME = eeprom_emulation_type_b_test

AFLAGS += -I.. \
    -S \
    --endian little \
    --thumb \
    "-I$(IAR_ARMCOMPILER)/inc/c" \
    --cpu Cortex-M33

DFLAGS += --debug

CFLAGS += -I.. \
    -D__MSPM33C321A__ \
    -Om \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050 \
    --cpu Cortex-M33


ifneq ($(CONFIG),RELEASE)
# Add debug flags by default (excluded only when CONFIG=RELEASE)
CFLAGS += $(DFLAGS)
endif

LFLAGS += --redirect \
    ScanfSmall=_ScanfSmall \
    "$(MSPM33_SDK_INSTALL_DIR)/source/ti/driverlib/lib/iar/m33/mspm33c321x/driverlib.a" \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L.. \
    --config ../iar/mspm33c321a.icf \
    --map "$(NAME).map" \
    --silent \
    "$(if $(filter RELEASE,$(CONFIG)),,--semihosting=iar_breakpoint)" \
    --cpu=Cortex-M33

all: $(NAME).out

main.obj: ../main.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

ti_eeprom_emulation_type_a_eeprom_emulation_type_a.obj: ../ti/eeprom/emulation_type_a/eeprom_emulation_type_a.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

ti_eeprom_emulation_type_b_eeprom_emulation_type_b.obj: ../ti/eeprom/emulation_type_b/eeprom_emulation_type_b.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

ti_msp_dl_config.obj: ../ti_msp_dl_config.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

iar_startup_mspm33c321x_iar.obj: ../iar/startup_mspm33c321x_iar.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	$(V) $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	$(V) $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).out > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).map > $(DEVNULL) 2>&1
