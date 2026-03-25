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

/*
 *  ============ secondary_bsl_empty.c =============
 *  Empty Bootloader Example
 *
 *  This example demonstrates how to implement an empty bootloader that
 *  jumps to an application located at a specific address (0x10080000).
 *
 *  The bootloader uses the GSC (Global Security Controller) VTOR mechanism
 *  to set the application's vector table address. The VTOR value is retained
 *  across SYSRST, so after the reset, execution begins at the application's
 *  reset vector.
 *
 *  Memory Layout (1MB total flash):
 *  - Bootloader:    0x10000000 - 0x1007FFFF (512KB)
 *  - Application:   0x10080000 - 0x100FFFFF (512KB)
 *
 *  Usage:
 *  1. Flash the bootloader via JTAG using "Erase Main and Nonmain memory"
 *  2. Flash the application via JTAG using "Do not erase Flash memory"
 *  3. On reset, bootloader executes and jumps to application
 */

#include "ti_msp_dl_config.h"

/*
 * Application start address - where the application's vector table is located.
 * This is 512KB offset from flash start (0x10000000 + 0x80000 = 0x10080000).
 * Adjust this value based on your memory layout requirements.
 *
 * Memory Layout (1MB total flash):
 * - Bootloader: 0x10000000 - 0x1007FFFF (512KB)
 * - Application: 0x10080000 - 0x100FFFFF (512KB)
 */
#define APPLICATION_START_ADDRESS (0x10080000U)

/*
 * Function: jumpToApplication
 *
 * Description:
 *   Sets the GSC VTOR register to the application's vector table address
 *   and issues a SYSRST to transfer execution to the application.
 *   The GSC VTOR is retained across SYSRST, so after reset the SCB VTOR
 *   will be loaded from GSC VTOR and execution begins at the application.
 *
 * Parameters:
 *   appAddress - Address where the application's vector table is located
 *
 * Note:
 *   This function does not return. After the reset, execution continues
 *   at the application's reset vector.
 */
static void jumpToApplication(uint32_t appAddress)
{
    /*
     * Set the application's vector table address in GSC VTOR register.
     * Use DL_GSC_setVTOR_S for secure application.
     * The VTOR value is retained across SYSRST.
     */
    DL_GSC_setVTOR_S(GSC, appAddress);

    /*
     * Issue a system reset (SYSRST - CPU plus peripherals only).
     * This is the appropriate reset type for custom bootloader implementations.
     * After reset, the SCB VTOR will be loaded from GSC VTOR and
     * execution will begin at the application's reset vector.
     */
    DL_SYSCTL_resetDevice(DL_SYSCTL_RESET_SYSRST);

    /*
     * Execution should never reach here.
     * If it does, enter an infinite loop for debugging.
     */
    while (1) {
        __WFI();
    }
}

/*
 * Function: validateApplication
 *
 * Description:
 *   Basic validation to check if a valid application exists at the
 *   specified address. Checks if the stack pointer value (first word
 *   of vector table) points to a valid RAM region.
 *
 * Parameters:
 *   appAddress - Address where the application's vector table is located
 *
 * Returns:
 *   true if application appears valid, false otherwise
 *
 * Note:
 *   This is a simple validation. For production use, consider adding
 *   CRC checks, digital signatures, or other integrity verification.
 */
static bool validateApplication(uint32_t appAddress)
{
    uint32_t stackPointer;

    /* Read the initial stack pointer from the application's vector table */
    stackPointer = *((uint32_t *) appAddress);

    /*
     * Check if stack pointer is within valid SRAM range.
     * SRAM on MSPM33C321A: 0x30000000 - 0x3003FFFF (256KB)
     */
    if ((stackPointer >= 0x30000000U) && (stackPointer <= 0x30040000U)) {
        return true;
    }

    return false;
}

int main(void)
{
    /*
     * Initialize system configuration.
     * This sets up clocks and any required peripherals.
     */
    SYSCFG_DL_init();

    /*
     * Optional: Add bootloader-specific logic here
     *
     * In a real bootloader implementation, you might:
     * - Check for firmware update request (button press, UART command, etc.)
     * - Verify application integrity (CRC, signature)
     * - Handle firmware update process
     * - Implement secure boot verification
     *
     * For this simple example, we just validate and jump to the application.
     */

    /*
     * Validate that a valid application exists at the target address.
     * If valid, jump to it. Otherwise, stay in bootloader (infinite loop).
     */
    if (validateApplication(APPLICATION_START_ADDRESS)) {
        /* Valid application found - jump to it */
        jumpToApplication(APPLICATION_START_ADDRESS);
    }

    /*
     * No valid application found.
     * In a real bootloader, you might:
     * - Wait for firmware update via UART/I2C/CAN
     * - Blink an LED to indicate error
     * - Enter low power mode
     *
     * For this example, we just loop forever.
     */
    while (1) {
        /* Toggle LED or perform other indication if available */
        __WFI();
    }
}
