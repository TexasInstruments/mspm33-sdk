MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(MSPM33_SDK_INSTALL_DIR)/kernel/freertos/builds/LP_MSPM33C321A/release

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler gcc --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler gcc --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../../blink_led.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = main.obj main-blinky.obj freertos_gcc_startup_mspm33c321x_gcc.obj freertos_gcc_system_mspm33c321x.obj $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

NAME = blink_led


DFLAGS += -gstrict-dwarf

CFLAGS += -I../.. \
    -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -D__MSPM33C321A__ \
    -O2 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/kernel/freertos/Source/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/kernel/freertos/Source/portable/GCC/ARM_CM33_NTZ/non_secure" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/ti/posix/gcc" \
    "-I$(KERNEL_BUILD)" \
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
    -Wl,-T,../../freertos/gcc/mspm33c321a.lds \
    "-Wl,-Map,$(NAME).map" \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L../.. \
    --specs=nosys.specs \
    "-L$(KERNEL_BUILD)/gcc" \
    -l:freertos.lib \
    -mcpu=cortex-m33 \
    -mthumb \
    -static \
    -Wl,--gc-sections \
    -lgcc \
    -lc \
    -lm \
    --specs=nano.specs

all: $(NAME).out

$(KERNEL_BUILD)/gcc/freertos.lib:
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


define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	$(V) $(CC) $(CFLAGS) $$< -c -o $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))

main.obj: ../../main.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

main-blinky.obj: ../../main-blinky.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

freertos_gcc_startup_mspm33c321x_gcc.obj: ../../freertos/gcc/startup_mspm33c321x_gcc.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

freertos_gcc_system_mspm33c321x.obj: ../../freertos/gcc/system_mspm33c321x.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) $< -c -o $@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/gcc/freertos.lib
	@ echo linking $@
	$(V) $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	$(V) $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).out > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).map > $(DEVNULL) 2>&1
	$(V) $(RM) $(call SLASH_FIXUP,$(SYSCFG_FILES)) > $(DEVNULL) 2>&1
