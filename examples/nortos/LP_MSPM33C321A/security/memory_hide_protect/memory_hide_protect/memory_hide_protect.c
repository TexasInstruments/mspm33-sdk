/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
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

#define TRANSITION_DELAY_CYCLES 4000000
#define TEST_DELAY_CYCLES 2000000

/* Sensitive data placed in HDP-protected flash region */
__attribute__((section(".protected_data")))
const uint32_t sensitive_key[8] = {0xDEADBEEF, 0xCAFEBABE, 0x12345678,
    0xABCDEF00, 0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00};

/* Sensitive algorithm placed in HDP-protected flash region */
__attribute__((section(".protected_code"))) uint32_t sensitive_algorithm(
    uint32_t input)
{
    return input ^ sensitive_key[0];
}

/* Simple delay function */
void delay_cycles(uint32_t cycles)
{
    while (cycles--) {
        __NOP();
    }
}

/* Strong NMI_Handler implementation that overrides the weak one in startup file
 * This handler is called when HDP protection triggers an NMI (Non-Maskable Interrupt) */
void NMI_Handler(void)
{
    /* Check EAM (Error Aggregator Module) to verify this is an HDP fault */
    DL_EAM_SECURITY_ERROR_FLAG eam_error_flag =
        DL_EAM_getSecurityErrorFlag(EAM);
    uint32_t eam_error_addr = DL_EAM_getSecurityErrorAddress(EAM);

    /* Get the address of the protected region that triggered the fault */
    uint32_t protected_region_addr_start = (uint32_t) &sensitive_key[0];
    uint32_t protected_region_addr_end   = (uint32_t) &sensitive_key[8];

    /* Verify this is an HDP error at the expected protected region address */
    if ((eam_error_flag & DL_EAM_SECURITY_ERROR_HDP) &&
        (eam_error_addr >= protected_region_addr_start) &&
        (eam_error_addr <= protected_region_addr_end)) {
        /* Confirmed: HDP protection fault occurred at protected region
         * Turn on all LEDs to indicate HDP fault occurred and test completed
         * GREEN LED (PA0) - Was on during successful access test
         * RED LED (PA26) - Was on when HDP protection was activated
         * BLUE LED (PA27) - Now turns on to show fault was caught
         * All three LEDs on = HDP protection demonstration complete */
        DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
        DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
        DL_GPIO_setPins(
            LED_GRP_TRANSITION_LED_PORT, LED_GRP_TRANSITION_LED_PIN);

        /* Clear the EAM security error flag */
        DL_EAM_clearSecurityErrorFlag(EAM);
    }

    /* Enter an infinite loop to preserve system state */
    while (1) {
        __WFI();
    }
}

/* Configure HDP (Hide Protection) for flash memory */
void configure_hdp(void)
{
    /* FLASH_PROTECTED region: 0x1000F000 - 0x1000F7FF (2KB)
     * Each sector is 2KB (0x800 bytes)
     * Sector calculation: address / 0x800
     * Start sector: 0x1000F000 / 0x800 = 30
     * End sector: 0x1000F7FF / 0x800 = 30 (same sector, but we use 30-30 range)
     */
    uint8_t hdp_start_sector = 30;
    uint8_t hdp_end_sector   = 30;

    /* Configure HDP region for MAIN flash bank 0 */
    bool result =
        DL_GSC_configureHDPRegion(GSC, hdp_start_sector, hdp_end_sector, 0);

    if (result) {
        /* HDP region configured successfully - blink GREEN LED */
        DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
        delay_cycles(TEST_DELAY_CYCLES);
        DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
        delay_cycles(TEST_DELAY_CYCLES);
    }

    /* Enable HDP mode globally */
    DL_GSC_enableHDPMode(GSC);

    /* Initially allow access to HDP region */
    DL_GSC_setHDPAccess(GSC, true, 0);

    /* Memory barriers to ensure configuration is complete */
    __ISB();
    __DSB();
}

/* Test 1: Access HDP-protected region with access enabled */
void test_hdp_access_enabled(void)
{
    /* Turn on GREEN LED to indicate test start */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Read from protected region - should succeed */
    volatile uint32_t value = sensitive_key[0];

    /* Call protected function - should succeed */
    volatile uint32_t result = sensitive_algorithm(0x12345678);

    /* Verify we read the correct value */
    if (value == 0xDEADBEEF && result != 0) {
        /* Success - blink GREEN LED rapidly */
        for (int i = 0; i < 10; i++) {
            DL_GPIO_togglePins(
                LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
            delay_cycles(TEST_DELAY_CYCLES / 6);
        }
    }

    DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

/* Test 2: Disable HDP access and attempt to read (triggers fault) */
void test_hdp_protection_active(void)
{
    /* Disable HDP access */
    DL_GSC_setHDPAccess(GSC, false, 0);
    __ISB();
    __DSB();

    /* Turn on RED LED to indicate HDP protection active */
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Attempt to read from protected region - this WILL trigger an NMI
     * The NMI_Handler will catch it and turn on all three LEDs to indicate
     * the HDP protection fault occurred. The handler enters an infinite loop,
     * preserving system state for debugging. */
    volatile uint32_t value = sensitive_key[0];

    /* This code will never execute because the fault handler loops forever */
    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

int main(void)
{
    /* Initialize peripherals and GPIO */
    SYSCFG_DL_init();

    /* Configure HDP protection for the sensitive data/code region
     * Protected region: 0x1000F000 - 0x1000F7FF (sector 30, 2KB)
     * Contains: sensitive_key[] and sensitive_algorithm() */
    configure_hdp();

    /* Brief startup delay */
    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* ===== HDP DEMONSTRATION SEQUENCE =====
     * This example demonstrates HDP (Hide Protection) functionality:
     *
     * 1. GREEN LED - Accessing protected data when HDP access is enabled
     *    Shows that with proper access rights, protected data can be read
     *    and the GREEN LED blinks rapidly to indicate successful access
     *
     * 2. RED LED + ALL LEDs - HDP protection triggering an NMI fault
     *    When HDP access is disabled, attempting to read protected memory
     *    triggers an NMI. The NMI_Handler turns on all three LEDs (GREEN,
     *    RED, and BLUE) to indicate the fault occurred and enters an
     *    infinite loop, preserving system state for debugging.
     */

    /* Test 1: Access with HDP enabled (should succeed) */
    test_hdp_access_enabled();
    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* Test 2: Disable access and attempt read (demonstrates protection)
     * This will trigger an NMI which is caught by NMI_Handler.
     * The handler will turn on all three LEDs and enter an infinite loop,
     * demonstrating that HDP protection is working. Execution will never
     * proceed past this point. */
    test_hdp_protection_active();

    /* Code below will never execute - included for documentation only */

    /* Final state: Keep HDP access enabled for safety
     *
     * In a real security application, you should:
     * 1. Enable HDP access only when the protected code/data is needed
     * 2. Disable it immediately after use: DL_GSC_setHDPAccess(GSC, false, 0)
     * 3. Implement proper fault handling for security violations
     * 4. Log or respond to unauthorized access attempts
     *
     * HDP provides hardware-enforced protection that cannot be bypassed,
     * making it ideal for protecting sensitive keys, algorithms, or IP.
     */

    while (1) {
        __WFI();
    }
}
