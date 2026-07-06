MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler ticlang --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler ticlang --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../generic_timeseries_classification_live_preview.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = generic_timeseries_classification_live_preview.obj ti_edgeAI_m33_feature_extract_feature_extract.obj ti_edgeAI_m33_feature_extract_feature_extract_m33.obj ti_edgeAI_m33_device_agent_protocol_Dap.obj ti_edgeAI_m33_device_agent_protocol_dap_core_Dap_Core.obj ti_edgeAI_m33_device_agent_protocol_dap_link_Dap_Link.obj ti_hal_mspm33_src_Uart_Uart_Hal.obj ti_hal_mspm33_src_Spi_Spi_Hal.obj ti_hal_mspm33_src_I2c_I2c_Hal_Common.obj ti_hal_mspm33_src_I2c_I2c_Hal_Controller.obj ti_hal_mspm33_src_I2c_I2c_Hal_Target.obj ti_hal_compiler_m33_ti_arm_clang_StrCmp_No_Dsp.obj $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

NAME = generic_timeseries_classification_live_preview


DFLAGS += -gdwarf-3

CFLAGS += -I.. \
    -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -DDEVICE_NAME=MSPM33C321A \
    -DARM_MATH_CM33 \
    -O0 \
    -Wall \
    -gdwarf-3 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    @device.opt \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/DSP/Include" \
    -I../artifacts \
    "-I$(MSPM33_SDK_INSTALL_DIR)/examples/edgeAI/generic_timeseries_classification_live_preview/LP_MSPM33C321A/artifacts" \
    -I../ti/edgeAI/m33/feature_extract \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    -mcpu=cortex-m33


ifneq ($(CONFIG),RELEASE)
# Add debug flags by default (excluded only when CONFIG=RELEASE)
CFLAGS += $(DFLAGS)
endif

LFLAGS += -L../artifacts \
    ../artifacts/mod.a \
    -ldevice.cmd.genlibs \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L.. \
    ../ticlang/device_linker.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../generic_timeseries_classification_live_preview.syscfg
	@ echo Generating configuration files...
	$(V) $(SYSCFG_CMD_STUB) --output $(@D) $<


# Helpful hint that the user needs to use a standalone SysConfig installation
$(SYSCONFIG_GUI_TOOL):
	$(error $(dir $(SYSCONFIG_TOOL)) does not contain the GUI framework \
        necessary to launch the SysConfig GUI.  Please set SYSCONFIG_TOOL \
        (in your SDK's imports.mak) to a standalone SysConfig installation \
        rather than one inside CCS)

syscfg-gui: ../generic_timeseries_classification_live_preview.syscfg $(SYSCONFIG_GUI_TOOL)
	@ echo Opening SysConfig GUI
	$(V) $(SYSCFG_GUI_CMD_STUB) $<


define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	$(V) $(CC) $(CFLAGS) -c $$< -o $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))

generic_timeseries_classification_live_preview.obj: ../generic_timeseries_classification_live_preview.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_edgeAI_m33_feature_extract_feature_extract.obj: ../ti/edgeAI/m33/feature_extract/feature_extract.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_edgeAI_m33_feature_extract_feature_extract_m33.obj: ../ti/edgeAI/m33/feature_extract/feature_extract_m33.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_edgeAI_m33_device_agent_protocol_Dap.obj: ../ti/edgeAI/m33/device_agent_protocol/Dap.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_edgeAI_m33_device_agent_protocol_dap_core_Dap_Core.obj: ../ti/edgeAI/m33/device_agent_protocol/dap_core/Dap_Core.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_edgeAI_m33_device_agent_protocol_dap_link_Dap_Link.obj: ../ti/edgeAI/m33/device_agent_protocol/dap_link/Dap_Link.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_hal_mspm33_src_Uart_Uart_Hal.obj: ../ti/hal/mspm33/src/Uart/Uart_Hal.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_hal_mspm33_src_Spi_Spi_Hal.obj: ../ti/hal/mspm33/src/Spi/Spi_Hal.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_hal_mspm33_src_I2c_I2c_Hal_Common.obj: ../ti/hal/mspm33/src/I2c/I2c_Hal_Common.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_hal_mspm33_src_I2c_I2c_Hal_Controller.obj: ../ti/hal/mspm33/src/I2c/I2c_Hal_Controller.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_hal_mspm33_src_I2c_I2c_Hal_Target.obj: ../ti/hal/mspm33/src/I2c/I2c_Hal_Target.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_hal_compiler_m33_ti_arm_clang_StrCmp_No_Dsp.obj: ../ti/hal/compiler/m33_ti_arm_clang/StrCmp_No_Dsp.c $(SYSCFG_H_FILES)
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
