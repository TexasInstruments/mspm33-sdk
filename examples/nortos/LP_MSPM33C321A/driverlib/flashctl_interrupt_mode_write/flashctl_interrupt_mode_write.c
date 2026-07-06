/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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

/* Base address of the sector to erase and program */
#define MAIN_BASE_ADDRESS   (0x00003000U)

/* End address (exclusive) of the sector to program: 2KB sector */
#define PROGRAM_END_ADDRESS (0x00003800U)

/* Size of one flash word: 128-bit = 16 bytes */
#define FLASH_WORD_SIZE     (16U)

/* 128-bit data pattern written to every flash word */
static const uint32_t gDataArray128[4] = {
    0xABCDEF00, 0x12345678, 0x98765432, 0x12345678
};

/* Tracks the next address to program */
volatile uint32_t gProgramAddress;

/* Operation states driven by FLASHCTL_IRQHandler */
typedef enum {
    STATE_ERASE_SECTOR,
    STATE_PROGRAM,
    STATE_DONE,
    STATE_ERROR
} FlashOpState;

volatile FlashOpState gFlashState;

int main(void)
{
    SYSCFG_DL_init();

    DL_GSC_acquireFlashSemaphore(GSC);

    /* Clear any stale flash status before starting */
    DL_FlashCTL_executeClearStatus(FLASHCTL);

    /* Enable FLASHCTL CMDDONE interrupt in NVIC */
    NVIC_EnableIRQ(FLASHCTL_INT_IRQn);

    /* Enable CMDDONE interrupt in the flash controller */
    DL_FlashCTL_enableInterrupt(FLASHCTL);

    gProgramAddress = MAIN_BASE_ADDRESS;
    gFlashState     = STATE_ERASE_SECTOR;

    /* Kick off the first operation: erase sector at 0x3000 (non-blocking) */
    DL_FlashCTL_unprotectSector(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
    DL_FlashCTL_eraseMemory(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_COMMAND_SIZE_SECTOR);

    /* Sleep until all operations complete or an error occurs */
    while (gFlashState != STATE_DONE && gFlashState != STATE_ERROR) {
        __WFE();
    }

    DL_FlashCTL_disableInterrupt(FLASHCTL);
    DL_GSC_clearFlashSemaphore(GSC);

    if (gFlashState == STATE_DONE) {
        /* All erase and program operations completed successfully */
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        /* An error occurred; inspect gFlashState and STATCMD for details */
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    __BKPT(0);
}

void FLASHCTL_IRQHandler(void)
{
    /* Clear the CMDDONE interrupt flag */
    DL_FlashCTL_clearInterruptStatus(FLASHCTL);

    /* Check if the last command failed */
    if (DL_FlashCTL_getCommandStatus(FLASHCTL) ==
        DL_FLASHCTL_COMMAND_STATUS_FAILED) {
        gFlashState = STATE_ERROR;
        return;
    }

    switch (gFlashState) {
        case STATE_ERASE_SECTOR:
            /* Sector erased; begin programming first word at 0x3000 */
            gFlashState = STATE_PROGRAM;
            DL_FlashCTL_unprotectSector(
                FLASHCTL, gProgramAddress, DL_FLASHCTL_REGION_SELECT_MAIN);
            DL_FlashCTL_programMemory128WithECCGenerated(
                FLASHCTL, gProgramAddress, &gDataArray128[0]);
            gProgramAddress += FLASH_WORD_SIZE;
            break;

        case STATE_PROGRAM:
            if (gProgramAddress < PROGRAM_END_ADDRESS) {
                uint32_t current_address = gProgramAddress;
                gProgramAddress += FLASH_WORD_SIZE;
                DL_FlashCTL_unprotectSector(FLASHCTL, current_address,
                    DL_FLASHCTL_REGION_SELECT_MAIN);
                DL_FlashCTL_programMemory128WithECCGenerated(
                    FLASHCTL, current_address, &gDataArray128[0]);
            } else {
                /* All 128 words (0x3000-0x37FF) programmed successfully */
                gFlashState = STATE_DONE;
            }
            break;

        default:
            break;
    }
}
