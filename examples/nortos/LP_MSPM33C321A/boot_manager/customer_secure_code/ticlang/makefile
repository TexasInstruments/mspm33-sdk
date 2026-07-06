MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

SYSCONFIG_GUI_TOOL = $(dir $(SYSCONFIG_TOOL))sysconfig_gui$(suffix $(SYSCONFIG_TOOL))
SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler ticlang --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_GUI_CMD_STUB = $(SYSCONFIG_GUI_TOOL) --compiler ticlang --product $(MSPM33_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../customer_secure_code.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = flash_map_backend_flash_map_backend.obj mcuboot_config_trace.obj customer_secure_code.obj boot_keys.obj lockable_storage_private.obj hash_verify.obj secret.obj keystore.obj rollback.obj third_party_mcuboot_boot_bootutil_src_boot_record.obj third_party_mcuboot_boot_bootutil_src_bootutil_misc.obj third_party_mcuboot_boot_bootutil_src_bootutil_public.obj third_party_mcuboot_boot_bootutil_src_caps.obj third_party_mcuboot_boot_bootutil_src_encrypted.obj third_party_mcuboot_boot_bootutil_src_fault_injection_hardening.obj third_party_mcuboot_boot_bootutil_src_fault_injection_hardening_delay_rng_mbedtls.obj third_party_mcuboot_boot_bootutil_src_image_ec.obj third_party_mcuboot_boot_bootutil_src_image_ec256.obj third_party_mcuboot_boot_bootutil_src_image_ed25519.obj third_party_mcuboot_boot_bootutil_src_image_mldsa87.obj third_party_mcuboot_boot_bootutil_src_image_rsa.obj third_party_mcuboot_boot_bootutil_src_image_validate.obj third_party_mcuboot_boot_bootutil_src_loader.obj third_party_mcuboot_boot_bootutil_src_swap_misc.obj third_party_mcuboot_boot_bootutil_src_swap_move.obj third_party_mcuboot_boot_bootutil_src_swap_scratch.obj third_party_mcuboot_boot_bootutil_src_tlv.obj third_party_mcuboot_ext_mbedtls-asn1_src_asn1parse.obj third_party_mcuboot_ext_mbedtls-asn1_src_platform_util.obj third_party_mcuboot_ext_msp-crypto_msp_crypto.obj ti_driverlib_dl_flashctl.obj ti_driverlib_dl_aesadv.obj ti_driverlib_m33_dl_shadrv.obj ti_driverlib_dl_dma.obj ti_boot_manager_ecc_src_ECCSW.obj ti_boot_manager_ecc_src_lowlevelapi.obj ti_boot_manager_ecc_src_lowlevelapi2.obj ti_boot_manager_ecc_src_lowlevelapi_m3.obj ti_boot_manager_ecc_src_scalarMul.obj ticlang_startup_mspm33c321x_ticlang.obj $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

NAME = customer_secure_code


DFLAGS += -gdwarf-3

CFLAGS += -I.. \
    -I. \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -D__MSPM33C321A__ \
    -DEXCLUDE_TRACE \
    -Oz \
    -flto \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/mcuboot/boot/bootutil/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/mcuboot/ext/mbedtls-asn1/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/mcuboot/ext/" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/ti/boot_manager/ecc/include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/ti/boot_manager/sha2sw/inc" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/ti/boot_manager/ecc/src" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    -mcpu=cortex-m33 \
    -Wall


ifneq ($(CONFIG),RELEASE)
# Add debug flags by default (excluded only when CONFIG=RELEASE)
CFLAGS += $(DFLAGS)
endif

LFLAGS += "-l$(MSPM33_SDK_INSTALL_DIR)/source/ti/boot_manager/lib/ticlang/m33/bimsupport.a" \
    "-l$(MSPM33_SDK_INSTALL_DIR)/source/ti/crypto/lib/ticlang/m33/mspm33c321x/cryptolib.a" \
    -Wl,--stack_size=256 \
    -Wl,--heap_size=128 \
    "-l$(MSPM33_SDK_INSTALL_DIR)/source/ti/driverlib/lib/ticlang/m33/mspm33c321x/driverlib.a" \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L.. \
    ../ticlang/mspm33c321a.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../customer_secure_code.syscfg
	@ echo Generating configuration files...
	$(V) $(SYSCFG_CMD_STUB) --output $(@D) $<


# Helpful hint that the user needs to use a standalone SysConfig installation
$(SYSCONFIG_GUI_TOOL):
	$(error $(dir $(SYSCONFIG_TOOL)) does not contain the GUI framework \
        necessary to launch the SysConfig GUI.  Please set SYSCONFIG_TOOL \
        (in your SDK's imports.mak) to a standalone SysConfig installation \
        rather than one inside CCS)

syscfg-gui: ../customer_secure_code.syscfg $(SYSCONFIG_GUI_TOOL)
	@ echo Opening SysConfig GUI
	$(V) $(SYSCFG_GUI_CMD_STUB) $<


define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	$(V) $(CC) $(CFLAGS) -c $$< -o $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))

flash_map_backend_flash_map_backend.obj: ../flash_map_backend/flash_map_backend.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

mcuboot_config_trace.obj: ../mcuboot_config/trace.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

customer_secure_code.obj: ../customer_secure_code.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

boot_keys.obj: ../boot_keys.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

lockable_storage_private.obj: ../lockable_storage_private.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

hash_verify.obj: ../hash_verify.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

secret.obj: ../secret.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

keystore.obj: ../keystore.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

rollback.obj: ../rollback.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_boot_record.obj: ../third_party/mcuboot/boot/bootutil/src/boot_record.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_bootutil_misc.obj: ../third_party/mcuboot/boot/bootutil/src/bootutil_misc.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_bootutil_public.obj: ../third_party/mcuboot/boot/bootutil/src/bootutil_public.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_caps.obj: ../third_party/mcuboot/boot/bootutil/src/caps.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_encrypted.obj: ../third_party/mcuboot/boot/bootutil/src/encrypted.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_fault_injection_hardening.obj: ../third_party/mcuboot/boot/bootutil/src/fault_injection_hardening.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_fault_injection_hardening_delay_rng_mbedtls.obj: ../third_party/mcuboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_image_ec.obj: ../third_party/mcuboot/boot/bootutil/src/image_ec.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_image_ec256.obj: ../third_party/mcuboot/boot/bootutil/src/image_ec256.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_image_ed25519.obj: ../third_party/mcuboot/boot/bootutil/src/image_ed25519.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_image_mldsa87.obj: ../third_party/mcuboot/boot/bootutil/src/image_mldsa87.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_image_rsa.obj: ../third_party/mcuboot/boot/bootutil/src/image_rsa.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_image_validate.obj: ../third_party/mcuboot/boot/bootutil/src/image_validate.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_loader.obj: ../third_party/mcuboot/boot/bootutil/src/loader.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_swap_misc.obj: ../third_party/mcuboot/boot/bootutil/src/swap_misc.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_swap_move.obj: ../third_party/mcuboot/boot/bootutil/src/swap_move.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_swap_scratch.obj: ../third_party/mcuboot/boot/bootutil/src/swap_scratch.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_boot_bootutil_src_tlv.obj: ../third_party/mcuboot/boot/bootutil/src/tlv.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_ext_mbedtls-asn1_src_asn1parse.obj: ../third_party/mcuboot/ext/mbedtls-asn1/src/asn1parse.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_ext_mbedtls-asn1_src_platform_util.obj: ../third_party/mcuboot/ext/mbedtls-asn1/src/platform_util.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_mcuboot_ext_msp-crypto_msp_crypto.obj: ../third_party/mcuboot/ext/msp-crypto/msp_crypto.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_driverlib_dl_flashctl.obj: ../ti/driverlib/dl_flashctl.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_driverlib_dl_aesadv.obj: ../ti/driverlib/dl_aesadv.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_driverlib_m33_dl_shadrv.obj: ../ti/driverlib/m33/dl_shadrv.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_driverlib_dl_dma.obj: ../ti/driverlib/dl_dma.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_boot_manager_ecc_src_ECCSW.obj: ../ti/boot_manager/ecc/src/ECCSW.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_boot_manager_ecc_src_lowlevelapi.obj: ../ti/boot_manager/ecc/src/lowlevelapi.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_boot_manager_ecc_src_lowlevelapi2.obj: ../ti/boot_manager/ecc/src/lowlevelapi2.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_boot_manager_ecc_src_scalarMul.obj: ../ti/boot_manager/ecc/src/scalarMul.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ticlang_startup_mspm33c321x_ticlang.obj: ../ticlang/startup_mspm33c321x_ticlang.c $(SYSCFG_H_FILES)
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_boot_manager_ecc_src_lowlevelapi_m3.obj: ../ti/boot_manager/ecc/src/lowlevelapi_m3.s $(SYSCFG_H_FILES)
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
