/*
 * Copyright (c) 2025, Texas Instruments Incorporated
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
#define MAIN_BASE_ADDRESS (0x00002000)

/* The address space in memory containing the programmed ECC code */
#define ECC_CODE_ADDRESS_SPACE (0x80200400)

/* The address space in memory containing the ECC-corrected data */
#define CORRECTED_ADDRESS_SPACE (0x00002000)

/*
 * Array to write 128-bits to flash. Data must be loaded 32-bits at a time,
 * but a single word program is executed.
 */
uint32_t gDataArray128[] = {0xABCDEF00, 0x12345678, 0x87654321, 0x00FEDCBA};

/* Array with "single-bit error" of gDataArray128 */
uint32_t gDataArray128SEC[] = {0xABCDEF01, 0x12345678, 0x87654321, 0x00FEDCBA};

/* Array with "double-bit error" of gDataArray128 */
uint32_t gDataArray128DED[] = {0xABCDEF11, 0x12345678, 0x87654321, 0x00FEDCBA};

volatile DL_FLASHCTL_COMMAND_STATUS gCmdStatus;

volatile bool gDEDFault = false;

uint32_t* gECCCodeAddress     = (void*) ECC_CODE_ADDRESS_SPACE;
uint32_t* gCorrectedAddress   = (void*) CORRECTED_ADDRESS_SPACE;
uint32_t gCorrectedData __attribute__((unused));
uint32_t gFaultStatus __attribute__((unused));
uint16_t gECCCode;

/* Local function prototype */
static void FlashCtl_eraseSectorBlocking(uint32_t address);

int main(void)
{
    SYSCFG_DL_init();

    DL_GSC_acquireFlashSemaphore(GSC);

    FlashCtl_eraseSectorBlocking(MAIN_BASE_ADDRESS);

    /*
     * Program 128-bit data with ECC enabled.
     * The flash controller hardware will handle generating the 16-bit ECC code.
     */
    DL_FlashCTL_unprotectSector(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
    gCmdStatus = DL_FlashCTL_programMemoryFromRAM128WithECCGenerated(
        FLASHCTL, (MAIN_BASE_ADDRESS), &gDataArray128[0]);
    if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
        /* If command did not pass, set a SW breakpoint. */
        __BKPT(0);
    }
    /* Get the 16-bit ECC code that was generated for this 128-bit data */
    gECCCode = *(gECCCodeAddress);

    FlashCtl_eraseSectorBlocking(MAIN_BASE_ADDRESS);

    /*
     * Enable override of hardware generated ECC. This allows the user to
     * manually set the ECC code to be programmed.
     */
    DL_FlashCTL_enableOverrideHardwareGeneratedECC(FLASHCTL);

    /*
     * Demonstrate a SEC (single bit error correction) error.
     * Program 128-bit data with ECC enabled, but the data in gDataArray128SEC
     * has a one bit difference from the data in gDataArray128.
     * Additionally, the ECC code being programmed corresponds with the data
     * in gDataArray128. Since we are using the ECC code corresponding with
     * gDataArray128, and gDataArray128SEC has a bit difference in the data,
     * this will trigger a SEC error.
     */
    DL_SYSCTL_clearECCErrorStatus();
    DL_FlashCTL_unprotectSector(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
    gCmdStatus = DL_FlashCTL_programMemoryFromRAM128WithECCManual(
        FLASHCTL, (MAIN_BASE_ADDRESS), &gDataArray128SEC[0], (uint8_t *)&gECCCode);
    if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
        /* If command did not pass, set a SW breakpoint. */
        __BKPT(0);
    }

    /*
     * The corrected address space has the ECC-corrected data.
     * Since there was only a single bit error, this is a correctable error, so
     * the 'corrected' data that matched the manually provided ECC code was
     * programmed. The 'corrected' data of gDataArray128SEC will match the data
     * of gDataArray128
     */
    gCorrectedData   = *gCorrectedAddress;
    /* Delay in order for register to update gFaultStatus accordingly */
    delay_cycles(100000);
    gFaultStatus = DL_SYSCTL_getStatus() & DL_SYSCTL_STATUS_FLASH_SEC;

    /*
     * Set a breakpoint to confirm the values of:
     * gFaultStatus = DL_SYSCTL_STATUS_FLASH_SEC
     * gCorrectedData = {0xABCDEF00, 0x12345678, 0x87654321, 0x00FEDCBA}
     * User can also check the memory browser at address
     * CORRECTED_ADDRESS_SPACE
     */

    FlashCtl_eraseSectorBlocking(MAIN_BASE_ADDRESS);

    /*
     * Demonstrate a DED (double bit error correction) error.
     * A DED error will result in an NMI (Non-Maskable Interrupt) triggering with a DED fault.
     * Program 128-bit data with ECC enabled, but the data in gDataArray128DED
     * has a two bit difference from the data in gDataArray128.
     * Additionally, the ECC code being programmed corresponds with the data in
     * gDataArray128. Since we are using the ECC code corresponding with
     * gDataArray128, and gDataArray128DED has a two bit difference in the data,
     * this will trigger a DED error.
    */
    DL_SYSCTL_clearECCErrorStatus();
    DL_FlashCTL_unprotectSector(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_REGION_SELECT_MAIN);
    gCmdStatus = DL_FlashCTL_programMemoryFromRAM128WithECCManual(
        FLASHCTL, (MAIN_BASE_ADDRESS), &gDataArray128DED[0], (uint8_t *)&gECCCode);
    if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
        /* If command did not pass, set a SW breakpoint. */
        __BKPT(0);
    }
    /* Reading the location will result in NMI triggered since ECC is incorrect */
    gCorrectedData = *gCorrectedAddress;

    DL_GSC_clearFlashSemaphore(GSC);

    while (gDEDFault == false) {
        __WFI();
    }
    __BKPT(0);
}

void NMI_Handler(void)
{
    switch (DL_SYSCTL_getPendingNonMaskableInterrupt()) {
        case DL_SYSCTL_NMI_IIDX_FLASH_DED:
            /*
            * NMI will trigger with DED fault.
            * The uncorrected address space has the original raw data of gDataArray128DED.
            * The corrected address space has the ECC-corrected data, but since there was
            * a double bit error and this type of error is uncorrectable, the data at
            * the corrected address space will also be the data of gDataArray128DED.
            * Confirm the values of:
            * gFaultStatus = DL_SYSCTL_STATUS_FLASH_DED
            * {0xABCDEF11, 0x12345678, 0x87654321, 0x00FEDCBA}
            */
            gCorrectedData   = *gCorrectedAddress;
            gFaultStatus = DL_SYSCTL_getStatus() & DL_SYSCTL_STATUS_FLASH_DED;
            gDEDFault    = true;

            DL_GPIO_setPins(GPIO_LEDS_PORT,
                GPIO_LEDS_PASS_LED_PIN);
            delay_cycles(1000);
            break;
        default:
            break;
    }
}

/*
 *  Helper function to perform erase of sector at the specified
 *  address. Function will block if command does not pass.
 *
 *  address   The address of sector to erase
 */
void FlashCtl_eraseSectorBlocking(uint32_t address)
{
    /* Erase memory before programming */
    DL_FlashCTL_unprotectSector(
        FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
    gCmdStatus = DL_FlashCTL_eraseMemoryFromRAM(
        FLASHCTL, MAIN_BASE_ADDRESS, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
    if (gCmdStatus != DL_FLASHCTL_COMMAND_STATUS_PASSED) {
        /* If command did not pass, set a SW breakpoint. */
        __BKPT(0);
    }
}
