/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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

#include "ti_msp_dl_config.h"

/* Address in main memory to write to */
#define MAIN_BASE_ADDRESS (0x00003000)

 /* 8-bit data to write to flash */
 uint8_t gData8 = 0x11;

 /* 16-bit data to write to flash */
 uint16_t gData16 = 0x2222;

 /* 32-bit data to write to flash */
 uint32_t gData32 = 0x33333333;

/* Array to write 64-bits to flash */
uint32_t gDataArray64[] = {0xABCDEF00, 0x12345678};

/* Array to write 96-bits to flash */
uint32_t gDataArray96[] = {0xABCDEF00, 0x12345678, 0x98765432};

/* Array to write 128-bits to flash */
uint32_t gDataArray128[] = {0xABCDEF00, 0x12345678, 0x98765432, 0x12345678};

/* "Incorrect" 128-bit data array to demonstrate read verify error */
uint32_t gDataArray128Error[] = {0x00FEDCBA, 0x87654321, 0x98765432, 0x12345678};

volatile DL_FLASHCTL_COMMAND_STATUS gCmdStatus;

#ifdef __SLATE_INTERNAL__
#define TEST_MEM_ERASED 1
#define TEST_COMPLETE 2
volatile uint8_t gTestFlow = 0;

#endif
volatile bool gCmdError = false;

/* Codes to understand where error occured */
#define NO_ERROR 0
#define ERROR_ERASE 1
#define ERROR_WRITE_8_BIT 2
#define ERROR_READ_8_BIT 3
#define ERROR_WRITE_16_BIT 4
#define ERROR_READ_16_BIT 5
#define ERROR_WRITE_32_BIT 6
#define ERROR_READ_32_BIT 7
#define ERROR_WRITE_64_BIT 8
#define ERROR_READ_64_BIT 9
#define ERROR_WRITE_96_BIT 10
#define ERROR_READ_96_BIT 11
#define ERROR_WRITE_128_BIT 12
#define ERROR_READ_128_BIT 13
#define ERROR_EXPECTED_FAIL 14
#define ERROR_UNEXPECTED 15

volatile uint8_t gErrorType = NO_ERROR;

int main(void)
{
    SYSCFG_DL_init();

    DL_GSC_acquireFlashSemaphore(GSC);

    DL_FLASHCTL_FAIL_TYPE failType;

    /* Erase sector in main memory */
    DL_FlashCTL_unprotectSector(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
    gCmdStatus = DL_FlashCTL_eraseMemoryFromRAM(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
    if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
        /* If command did not pass, set error flag. */
        gErrorType = ERROR_ERASE;
    }

    if (gErrorType == NO_ERROR) {
        /* 8-bit write to flash in main memory */
        DL_FlashCTL_unprotectSector(
            FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
        gCmdStatus = DL_FlashCTL_programMemoryFromRAM8(
            FLASHCTL, MAIN_BASE_ADDRESS, &gData8);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_WRITE_8_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* Read verify on the 8-bit data */
        gCmdStatus = DL_FlashCTL_readVerifyFromRAM8(
            FLASHCTL, MAIN_BASE_ADDRESS, &gData8);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_READ_8_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* 16-bit write to flash in main memory  */
        DL_FlashCTL_unprotectSector(
            FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
        gCmdStatus = DL_FlashCTL_programMemoryFromRAM16(
            FLASHCTL, (MAIN_BASE_ADDRESS + 16), &gData16);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_WRITE_16_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* Read verify on the 16-bit data */
        gCmdStatus = DL_FlashCTL_readVerifyFromRAM16(
            FLASHCTL, (MAIN_BASE_ADDRESS + 16), &gData16);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_READ_16_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* 32-bit write to flash in main memory */
        DL_FlashCTL_unprotectSector(
            FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
        gCmdStatus = DL_FlashCTL_programMemoryFromRAM32(
            FLASHCTL, (MAIN_BASE_ADDRESS + 32), &gData32);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_WRITE_32_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* Read verify on the 32-bit data */
        gCmdStatus = DL_FlashCTL_readVerifyFromRAM32(
            FLASHCTL, (MAIN_BASE_ADDRESS + 32), &gData32);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_READ_32_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* 64-bit write to flash in main memory */
        DL_FlashCTL_unprotectSector(
            FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
        gCmdStatus = DL_FlashCTL_programMemoryFromRAM64(
            FLASHCTL, (MAIN_BASE_ADDRESS + 48), &gDataArray64[0]);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_WRITE_64_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* Read verify on the 64-bit data */
        gCmdStatus = DL_FlashCTL_readVerifyFromRAM64(
            FLASHCTL, (MAIN_BASE_ADDRESS + 48), &gDataArray64[0]);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_READ_64_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* 96-bit write to flash in main memory */
        DL_FlashCTL_unprotectSector(
            FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
        gCmdStatus = DL_FlashCTL_programMemoryFromRAM96(
            FLASHCTL, (MAIN_BASE_ADDRESS + 64), &gDataArray96[0]);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_WRITE_96_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* Read verify on the 96-bit data */
        gCmdStatus = DL_FlashCTL_readVerifyFromRAM96(
            FLASHCTL, (MAIN_BASE_ADDRESS + 64), &gDataArray96[0]);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_READ_96_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* 128-bit write to flash in main memory */
        DL_FlashCTL_unprotectSector(
            FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
        gCmdStatus = DL_FlashCTL_programMemoryFromRAM128(
            FLASHCTL, (MAIN_BASE_ADDRESS + 80), &gDataArray128[0]);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_WRITE_128_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* Read verify on the 128-bit data */
        gCmdStatus = DL_FlashCTL_readVerifyFromRAM128(
            FLASHCTL, (MAIN_BASE_ADDRESS + 80), &gDataArray128[0]);
        if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
            /* If command did not pass, set error flag. */
            gErrorType = ERROR_READ_128_BIT;
        }
    }

    if (gErrorType == NO_ERROR) {
        /* Demonstrate a failed read verify when data does not match */
        gCmdStatus = DL_FlashCTL_readVerifyFromRAM128(
            FLASHCTL, (MAIN_BASE_ADDRESS + 80), &gDataArray128Error[0]);
        failType = DL_FlashCTL_getFailureStatus(FLASHCTL);
        if ((gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_FAILED)) {
            /* If command did not fail, set error flag. */
            gErrorType = ERROR_EXPECTED_FAIL;
        } else if ((failType != DL_FLASHCTL_FAIL_TYPE_VERIFY_ERROR)) {
            /* If there is an unexpected failure, set error flag */
            gErrorType = ERROR_UNEXPECTED;
        }
    }

    DL_GSC_clearFlashSemaphore(GSC);

    /* After successful completion, set PASS LED */
    if (gErrorType == NO_ERROR) {
        DL_GPIO_setPins(GPIO_LEDS_PORT,
            GPIO_LEDS_PASS_LED_PIN);
        delay_cycles(1000);
    }
    /* Unsuccessful example run, set FAIL LED */
    else {
        DL_GPIO_setPins(GPIO_LEDS_PORT,
            GPIO_LEDS_FAIL_LED_PIN);
        delay_cycles(1000);
        /* Check gErrorType value */
    }
    __BKPT(0);
}
