MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(MSPM33_SDK_INSTALL_DIR)/kernel/freertos/builds/LP_MSPM33C321A/release

ASM = "$(IAR_ARMCOMPILER)/bin/iasmarm"
CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler iar --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler iar --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../../blink_led.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = main.obj main-blinky.obj freertos_iar_startup_mspm33c321x_iar.obj $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))
SYSCFG_ASM_FILES = $(filter %.s,$(SYSCFG_FILES))
OBJECTS += $(patsubst %.s,%.obj,$(notdir $(SYSCFG_ASM_FILES)))


NAME = blink_led

AFLAGS += -I../.. \
    "-I$(KERNEL_BUILD)" \
    -S \
    --endian little \
    --thumb \
    "-I$(IAR_ARMCOMPILER)/inc/c" \
    --cpu Cortex-M33

DFLAGS += --debug

CFLAGS += -I../.. \
    -I. \
    $(addprefix -f,$(SYSCFG_OPT_FILES)) \
    -D__MSPM33C321A__ \
    -Om \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/kernel/freertos/Source/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/kernel/freertos/Source/portable/IAR/ARM_CM33_NTZ/non_secure" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/ti/posix/iar" \
    "-I$(KERNEL_BUILD)" \
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
    -L../.. \
    "$(KERNEL_BUILD)/iar/freertos.lib" \
    --config ../../freertos/iar/mspm33c321a.icf \
    --map "$(NAME).map" \
    --silent \
    "$(if $(filter RELEASE,$(CONFIG)),,--semihosting=iar_breakpoint)" \
    --cpu=Cortex-M33

all: $(NAME).out

$(KERNEL_BUILD)/iar/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../../blink_led.syscfg
	@ echo Generating configuration files...
	$(V) $(SYSCFG_CMD_STUB) --output $(@D) $<


# Helpful hint that the user needs to use a standalone SysConfig installation
$(SYSCONFIG_GUI_TOOL):
	$(error $(dir $(SYSCONFIG_TOOL)) does not contain the GUI framework \
        necessary to launch the SysConfig GUI.  Please set SYSCONFIG_TOOL \
        (in your SDK's imports.mak) to a standalone SysConfig installation \
        rather than one inside CCS)

syscfg-gui: ../../blink_led.syscfg $(SYSCONFIG_GUI_TOOL)
	@ echo Opening SysConfig GUI
	$(V) $(SYSCFG_GUI_CMD_STUB) $<

define ASM_RULE
$(basename $(notdir $(1))).obj: $(1)
	@ echo Building $$@
	$(V) $(ASM) $(AFLAGS) $$< -o $$@
endef

$(foreach asm_file,$(SYSCFG_ASM_FILES),$(eval $(call ASM_RULE,$(asm_file))))

define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	$(V) $(CC) $(CFLAGS) $$< -o $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))

main.obj: ../../main.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

main-blinky.obj: ../../main-blinky.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

freertos_iar_startup_mspm33c321x_iar.obj: ../../freertos/iar/startup_mspm33c321x_iar.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/iar/freertos.lib
	@ echo linking $@
	$(V) $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	$(V) $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).out > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).map > $(DEVNULL) 2>&1
	$(V) $(RM) $(call SLASH_FIXUP,$(SYSCFG_FILES)) > $(DEVNULL) 2>&1
