/*
 * Copyright (c) 2021-2026, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "boot_config.h"
#include "ti_msp_dl_config.h"

/* clang-format off */

/* Base address of nonmain sector containing BCR+BSL memory */
#define NONMAIN_SECTOR_BASE_ADDRESS                                      (0x80101800U)

/* Base address of the BCR configuration structure in nonmain memory */
#define BCR_USER_CFG_BASE_ADDRESS                                 (0x80101800U)

/* Base address of the BSL configuration structure in nonmain memory */
#define BSL_USER_CFG_BASE_ADDRESS                                 (0x80101C00U)

/* Size in bytes of the BSL and BCR configuration structures */
#define BCR_CONFIG_SIZE_BYTES                                             (336U)
#define BSL_CONFIG_SIZE_BYTES                                             (80U)

/* The calculated CRC based on the default configuration values */
#define BCR_CFG_DEFAULT_CRC                                        (0x7B106534)
#define BSL_CFG_DEFAULT_CRC                                        (0xB4808AA4)

volatile DL_FLASHCTL_COMMAND_STATUS gCmdStatus;

/* Codes to understand where error occured */
#define NO_ERROR 0
#define ERROR_ERASE 1
#define ERROR_PROGRAM_BCR 2
#define ERROR_PROGRAM_BSL 3


volatile uint8_t gErrorType = NO_ERROR;

/* clang-format on */

/* The default configuration of the BCR config struct */
BCR_Config gBCRConfig = {.BCR_CONFIG_ID = 0x05000000,
    .DEBUG_ACCESS                       = BC_CFG_DEBUG_L0,
    .SWDP_MODE                          = BC_CFG_SW_DP_EN,
    .BSL_PIN_INVOKE                     = BC_CFG_BSL_PIN_INVOKE_EN,
    .DEBUG_HOLD                         = BC_CFG_DEBUG_HOLD_DIS,
    .CSC_EXISTS                         = BC_CFG_CSC_NOT_EXIST,
    .FLASH_BANK_SWAP_POLICY             = BC_CFG_FLASH_BS_DIS,
    .FAST_BOOT_MODE                     = BC_CFG_FAST_BOOT_DIS,

    .SERVICES =
        {
            .BOOTLOADER_MODE    = BC_CFG_BOOTLOADER_MODE_EN,
            .MASS_ERASE_MODE    = BC_CFG_MASS_ERASE_EN,
            .FACTORY_RESET_MODE = BC_CFG_FACTORY_RESET_EN,
        },

    .PASSWORDS =
        {
            .MASS_ERASE =
                {
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                },
            .FACTORY_RESET =
                {
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                },
            .DEBUG_LOCK =
                {
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                },
            .DEBUG_NS_LOCK =
                {
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                    CFG_DEFAULT_VALUE,
                },
            .RSVD_PSWD = {CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE,
                CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE,
                CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE},
        },

    .SECURE_BOOT_MODE           = BC_CFG_SECURE_BOOT_DIS,
    .BC_RESERVED_0              = 0xFFFF,
    .USER_SECURE_APP_START_ADDR = CFG_DEFAULT_VALUE,
    .USER_SECURE_APP_LENGTH     = CFG_DEFAULT_VALUE,
    .USER_SECURE_APP_HASH       = {CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE,
        CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE,
        CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE, CFG_DEFAULT_VALUE},

    .GSC_CONFIGS = {.BANK0_NM_USER_CONFIG = BC_CFG_NONMAIN_WRITE_PROTECT_DIS,
        .BC_RESERVED_1                    = 0xFFFF,
        .BANK0 = {.WRITE_ERASE_PROTECTION_A = 0xFFFFFFFFUL,
            .WRITE_ERASE_PROTECTION_B       = 0xFFFFFFFFUL,
            .SECURITY_PROTECTION_A          = 0x00000000UL,
            .SECURITY_PROTECTION_B          = 0x00000000UL,
            .PRIVILEGE_PROTECTION_A         = 0x00000000UL,
            .PRIVILEGE_PROTECTION_B         = 0x00000000UL,
            .reserved                       = 0xFFFFFFFF},
        .BANK1 = {.WRITE_ERASE_PROTECTION_A = 0xFFFFFFFFUL,
            .WRITE_ERASE_PROTECTION_B       = 0xFFFFFFFFUL,
            .SECURITY_PROTECTION_A          = 0x00000000UL,
            .SECURITY_PROTECTION_B          = 0x00000000UL,
            .PRIVILEGE_PROTECTION_A         = 0x00000000UL,
            .PRIVILEGE_PROTECTION_B         = 0x00000000UL,
            .reserved                       = 0xFFFFFFFF},
        .DBANK =
            {/* For Write/Erase Protection:
             * Upper 16 bits: 0xFFFF (default/unprogrammed state)
             * Lower 16 bits: Protection configuration
             */
                .WRITE_ERASE_PROTECTION = 0xFFFFFFFFUL,

                /* For Security Protection:
             * Upper 16 bits: 0x0000 (secure)
             * Lower 16 bits: Protection configuration
             */
                .SECURITY_PROTECTION = 0x00000000UL,

                /* For Privilege Protection:
             * Upper 16 bits: 0x0000 (privileged)
             * Lower 16 bits: Protection configuration
             */
                .PRIVILEGE_PROTECTION = 0x00000000UL,

                .reserved = 0xFFFFFFFF}},

    .SYSPLL_CONFIG             = 0xFF,
    .SYSPLL_CONFIG_QDIV        = 0xf,
    .SYSPLL_CONFIG_RDIV        = 0x0,
    .SYSPLL_SOURCE             = 0xFF,
    .SYSPLL_SETTLING_TIME      = 0xc8,
    .HFXT_STARTUP_MONITOR_TIME = 0x8,

    .RESVD = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
        0xFFFF, 0xFFFF, 0xFFFF},

    .CRC = 0x7b106534};

/* The default configuration of the BSL config struct */
BSL_Config gBSLConfig = {.BSL_CONFIG_ID = 0x5000000,
    .BLINTERFACE_PINS = {.UART_RXD_pad_num = DEF_UART_RXD_PAD,
        .UART_RXD_PF_mux_sel               = DEF_UART_RXD_MUX,
        .UART_TXD_pad_num                  = DEF_UART_TXD_PAD,
        .UART_TXD_PF_mux_sel               = DEF_UART_TXD_MUX,
        .I2C_SDA_pad_num                   = DEF_I2C_SDA_PAD,
        .I2C_SDA_PF_mux_sel                = DEF_I2C_SDA_MUX,
        .I2C_SCL_pad_num                   = DEF_I2C_SCL_PAD,
        .I2C_SCL_PF_mux_sel                = DEF_I2C_SCL_MUX,
        .MCAN_RX_pad_num                   = DEF_MCAN_RXD_PAD,
        .MCAN_RX_PF_mux_sel                = DEF_MCAN_RXD_MUX,
        .MCAN_TX_pad_num                   = DEF_MCAN_TXD_PAD,
        .MCAN_TX_PF_mux_sel                = DEF_MCAN_TXD_MUX},
    .BSLPIN_INVOKE    = {.Pin_data_0 = DEFAULT_BSL_PIN_INVOCATION_DATA0,
        .Pin_data_1               = DEFAULT_BSL_PIN_INVOCATION_DATA1},
    .READOUT          = BSL_CFG_MEMORY_READOUT_ENABLE,
    .PASSWORD = {0x761396af, 0x5f63720f, 0x5a4ab4bd, 0x9fc3630a, 0xf930af12,
        0x5ceea650, 0x88e11b97, 0x51409ce8},
    .APP_REV_POINTER      = (uint32_t *) CFG_DEFAULT_VALUE,
    .SECURITY_ALERT_LEVEL = BSL_CFG_SECURITY_FACTORY_RESET,
    .UART_BAUD_RATE       = BL_CFG_UART_BAUDRATE_9600,
    .I2C_SLAVE_ADDR       = 0x48,
    .RESERVED = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF},
    .CRC      = 0xb4808aa4};

uint32_t calcUserConfigCRC(uint8_t *dataPointer, uint32_t size)
{
    static uint32_t i;

    uint32_t calculatedCRC;

    DL_CRCP_init(CRCP0, DL_CRCP_POLYNOMIAL_SIZE_32, DL_CRCP_BIT_REVERSED,
        DL_CRCP_INPUT_ENDIANESS_LITTLE_ENDIAN,
        DL_CRCP_OUTPUT_BYTESWAP_DISABLED);

    /* Set the Seed value to reset the calculation */
    DL_CRCP_setSeed32(CRCP0, CRCP_SEED);

    for (i = (uint32_t) 0U; i < size; i++) {
        DL_CRCP_feedData8(CRCP0, dataPointer[i]);
    }
    calculatedCRC = DL_CRCP_getResult32(CRCP0);

    return calculatedCRC;
}

int main(void)
{
    SYSCFG_DL_init();

    DL_GSC_acquireFlashSemaphore(GSC);

    /*
     * Calculate the CRC for the BCR and BSL configuration structs
     * The CRC is 4 bytes in each struct, this should not be included in the calculation
     */
    gBCRConfig.CRC = calcUserConfigCRC((uint8_t *) &gBCRConfig,
        BCR_CONFIG_SIZE_BYTES - ((uint32_t) sizeof(gBCRConfig.CRC)));
    gBSLConfig.CRC = calcUserConfigCRC((uint8_t *) &gBSLConfig,
        BSL_CONFIG_SIZE_BYTES - ((uint32_t) sizeof(gBSLConfig.CRC)));

    /* Unprotect and then erase NONMAIN memory */
    DL_FlashCTL_unprotectSector(FLASHCTL, NONMAIN_SECTOR_BASE_ADDRESS,
        DL_FLASHCTL_REGION_SELECT_NONMAIN);
    gCmdStatus = DL_FlashCTL_eraseMemoryFromRAM(FLASHCTL, NONMAIN_SECTOR_BASE_ADDRESS,
        DL_FLASHCTL_COMMAND_SIZE_SECTOR);

    if (gCmdStatus == DL_FLASHCTL_COMMAND_STATUS_FAILED) {
        /* If command was not successful, set error flag */
        gErrorType = ERROR_ERASE;
    }

    if (gErrorType == NO_ERROR) {
        /*
        * Program the BCR config structure to NONMAIN memory.
        */
        DL_FlashCTL_unprotectSector(FLASHCTL, NONMAIN_SECTOR_BASE_ADDRESS,
            DL_FLASHCTL_REGION_SELECT_NONMAIN);
        gCmdStatus =
            DL_FlashCTL_programMemoryBlockingFromRAM128WithECCGenerated(
                FLASHCTL, BCR_USER_CFG_BASE_ADDRESS, (uint32_t *) &gBCRConfig,
                (BCR_CONFIG_SIZE_BYTES / 4),
                DL_FLASHCTL_REGION_SELECT_NONMAIN);
        if (gCmdStatus == DL_FLASHCTL_COMMAND_STATUS_FAILED) {
            /* If command was not successful, set error flag */
            gErrorType = ERROR_PROGRAM_BCR;
        }
    }

    if (gErrorType == NO_ERROR) {
        /*
        * Program the BSL config structure to NONMAIN memory.
        */
        DL_FlashCTL_unprotectSector(FLASHCTL, NONMAIN_SECTOR_BASE_ADDRESS,
            DL_FLASHCTL_REGION_SELECT_NONMAIN);
        gCmdStatus =
            DL_FlashCTL_programMemoryBlockingFromRAM128WithECCGenerated(
                FLASHCTL, BSL_USER_CFG_BASE_ADDRESS, (uint32_t *) &gBSLConfig,
                (BSL_CONFIG_SIZE_BYTES / 4),
                DL_FLASHCTL_REGION_SELECT_NONMAIN);

        if (gCmdStatus == DL_FLASHCTL_COMMAND_STATUS_FAILED) {
            /* If command was not successful, set error flag */
            gErrorType = ERROR_PROGRAM_BSL;
        }
    }

    DL_GSC_clearFlashSemaphore(GSC);

    /* After successful completion, set PASS LED */
    if (gErrorType == NO_ERROR) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
        delay_cycles(1000);
    }
    /* Unsuccessful example run, set FAIL LED */
    else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
        delay_cycles(1000);
        /* Check gErrorType value */
    }
    __BKPT(0);
}
