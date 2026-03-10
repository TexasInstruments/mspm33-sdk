SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SDK_INSTALL_DIR)/imports.mak

CC  = "$(IAR_ARMCOMPILER)/bin/iccarm"
ASM = "$(IAR_ARMCOMPILER)/bin/iasmarm"
AR  = "$(IAR_ARMCOMPILER)/bin/iarchive"

NAME = driverlib

OBJ_DIR = ../../../../obj/iar/m33/mspm33c321x/driverlib
SRC_DIR = ../../../..

CFLAGS = "-I$(IAR_ARMCOMPILER)/inc/c" "-I$(SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" "-I$(SDK_INSTALL_DIR)/source" --cpu Cortex-M33 -D__MSPM33C321A__ --aeabi --guard_calls --endian=little --thumb --silent -e --use_unix_directory_separators --diag_suppress=Pa050,Go005
AFLAGS = 
ASMFLAGS = "-I$(IAR_ARMCOMPILER)/inc/c" 

OBJECTS = $(OBJ_DIR)/dl_aesadv.o $(OBJ_DIR)/dl_common.o $(OBJ_DIR)/dl_comp.o $(OBJ_DIR)/dl_crcp.o $(OBJ_DIR)/dl_dma.o $(OBJ_DIR)/dl_flashctl.o $(OBJ_DIR)/dl_gpio.o $(OBJ_DIR)/dl_gsc.o $(OBJ_DIR)/dl_hsadc.o $(OBJ_DIR)/dl_i2s.o $(OBJ_DIR)/dl_iwdt.o $(OBJ_DIR)/dl_keystorectl.o $(OBJ_DIR)/dl_lfss.o $(OBJ_DIR)/dl_mcan.o $(OBJ_DIR)/dl_qspi.o $(OBJ_DIR)/dl_rtc.o $(OBJ_DIR)/dl_rtc_a.o $(OBJ_DIR)/dl_rtc_b.o $(OBJ_DIR)/dl_rtc_common.o $(OBJ_DIR)/dl_scratchpad.o $(OBJ_DIR)/dl_spgss.o $(OBJ_DIR)/dl_tamperio.o $(OBJ_DIR)/dl_timer.o $(OBJ_DIR)/dl_timera.o $(OBJ_DIR)/dl_timerg.o $(OBJ_DIR)/dl_trng.o $(OBJ_DIR)/dl_uart.o $(OBJ_DIR)/dl_unicomm.o $(OBJ_DIR)/dl_unicommi2cc.o $(OBJ_DIR)/dl_unicommi2ct.o $(OBJ_DIR)/dl_unicommspi.o $(OBJ_DIR)/dl_unicommuart.o $(OBJ_DIR)/dl_vref.o $(OBJ_DIR)/dl_wwdt.o $(OBJ_DIR)/dl_eam.o $(OBJ_DIR)/dl_interrupt.o $(OBJ_DIR)/dl_pka.o $(OBJ_DIR)/dl_shadrv.o $(OBJ_DIR)/dl_sysctl_mspm33c321x.o

all: $(NAME).a

$(NAME).a: $(OBJECTS)
	@ echo Archiving $@
	@ $(AR) $(AFLAGS) $@ $^

$(OBJ_DIR)/dl_aesadv.o: $(SRC_DIR)/dl_aesadv.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_common.o: $(SRC_DIR)/dl_common.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_comp.o: $(SRC_DIR)/dl_comp.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_crcp.o: $(SRC_DIR)/dl_crcp.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_dma.o: $(SRC_DIR)/dl_dma.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_flashctl.o: $(SRC_DIR)/dl_flashctl.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_gpio.o: $(SRC_DIR)/dl_gpio.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_gsc.o: $(SRC_DIR)/dl_gsc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_hsadc.o: $(SRC_DIR)/dl_hsadc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_i2s.o: $(SRC_DIR)/dl_i2s.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_iwdt.o: $(SRC_DIR)/dl_iwdt.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_keystorectl.o: $(SRC_DIR)/dl_keystorectl.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_lfss.o: $(SRC_DIR)/dl_lfss.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_mcan.o: $(SRC_DIR)/dl_mcan.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_qspi.o: $(SRC_DIR)/dl_qspi.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_rtc.o: $(SRC_DIR)/dl_rtc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_rtc_a.o: $(SRC_DIR)/dl_rtc_a.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_rtc_b.o: $(SRC_DIR)/dl_rtc_b.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_rtc_common.o: $(SRC_DIR)/dl_rtc_common.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_scratchpad.o: $(SRC_DIR)/dl_scratchpad.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_spgss.o: $(SRC_DIR)/dl_spgss.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_tamperio.o: $(SRC_DIR)/dl_tamperio.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_timer.o: $(SRC_DIR)/dl_timer.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_timera.o: $(SRC_DIR)/dl_timera.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_timerg.o: $(SRC_DIR)/dl_timerg.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_trng.o: $(SRC_DIR)/dl_trng.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_uart.o: $(SRC_DIR)/dl_uart.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_unicomm.o: $(SRC_DIR)/dl_unicomm.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_unicommi2cc.o: $(SRC_DIR)/dl_unicommi2cc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_unicommi2ct.o: $(SRC_DIR)/dl_unicommi2ct.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_unicommspi.o: $(SRC_DIR)/dl_unicommspi.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_unicommuart.o: $(SRC_DIR)/dl_unicommuart.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_vref.o: $(SRC_DIR)/dl_vref.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_wwdt.o: $(SRC_DIR)/dl_wwdt.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_eam.o: $(SRC_DIR)/m33/dl_eam.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_interrupt.o: $(SRC_DIR)/m33/dl_interrupt.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_pka.o: $(SRC_DIR)/m33/dl_pka.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_shadrv.o: $(SRC_DIR)/m33/dl_shadrv.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/dl_sysctl_mspm33c321x.o: $(SRC_DIR)/m33/sysctl/dl_sysctl_mspm33c321x.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
