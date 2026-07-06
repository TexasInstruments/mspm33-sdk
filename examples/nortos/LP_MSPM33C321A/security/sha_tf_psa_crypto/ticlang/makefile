MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler ticlang --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler ticlang --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../sha_tfpsa.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = sha_tfpsa.obj $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

NAME = sha_tf_psa_crypto


DFLAGS += -gdwarf-3

CFLAGS += -I.. \
    -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    "-DTF_PSA_CRYPTO_CONFIG_FILE=<crypto-config-m33-baremetal.h>" \
    -DCRYPTO_HW_USE_MSPM33_SHA \
    -DCRYPTO_HW_USE_MSPM33_AES \
    -O2 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/drivers/builtin/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/drivers/builtin/src" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/utilities" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/core" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/configs" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/dispatch" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/platform" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/extras" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/drivers/mspm33/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    -mcpu=cortex-m33 \
    -Wall


ifneq ($(CONFIG),RELEASE)
# Add debug flags by default (excluded only when CONFIG=RELEASE)
CFLAGS += $(DFLAGS)
endif

LFLAGS += -ldevice.cmd.genlibs \
    "-l$(MSPM33_SDK_INSTALL_DIR)/source/third_party/tf-psa-crypto/lib/ticlang/m33/libtfpsacrypto.a" \
    "-l$(MSPM33_SDK_INSTALL_DIR)/source/ti/driverlib/lib/ticlang/m33/mspm33c321x/driverlib.a" \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L.. \
    device_linker.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../sha_tfpsa.syscfg
	@ echo Generating configuration files...
	$(V) $(SYSCFG_CMD_STUB) --output $(@D) $<


# Helpful hint that the user needs to use a standalone SysConfig installation
$(SYSCONFIG_GUI_TOOL):
	$(error $(dir $(SYSCONFIG_TOOL)) does not contain the GUI framework \
        necessary to launch the SysConfig GUI.  Please set SYSCONFIG_TOOL \
        (in your SDK's imports.mak) to a standalone SysConfig installation \
        rather than one inside CCS)

syscfg-gui: ../sha_tfpsa.syscfg $(SYSCONFIG_GUI_TOOL)
	@ echo Opening SysConfig GUI
	$(V) $(SYSCFG_GUI_CMD_STUB) $<


define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	$(V) $(CC) $(CFLAGS) -c $$< -o $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))

sha_tfpsa.obj: ../sha_tfpsa.c $(SYSCFG_H_FILES)
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
	$(V) $(RM) $(call SLASH_FIXUP,$(SYSCFG_FILES)) > $(DEVNULL) 2>&1
