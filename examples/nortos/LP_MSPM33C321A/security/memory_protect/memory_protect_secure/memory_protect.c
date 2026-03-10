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

#include "config.h"
#include "partition.h"
#include "sns_helpers.h"
#include "ti_msp_dl_config.h"

#define TRANSITION_DELAY_CYCLES 100000000
#define TEST_DELAY_CYCLES 50000000

/* Secure data buffer in secure SRAM - contains sensitive data */
volatile uint32_t secure_data_buffer[16] = {0x00000000, 0x12345678, 0xABCDEF00,
    0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00, 0x01020304, 0x05060708,
    0x090A0B0C, 0x0D0E0F10, 0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
    0x21222324};

/* Pointer to non-secure SRAM for cross-domain access testing */
volatile uint32_t *ns_sram_ptr =
    (volatile uint32_t *) NON_SECURE_APP0_SRAM_START;

/* Configure Security Attribution Unit (SAU) for memory region security */
void configure_sau(void)
{
    /* Disable SAU */
    SAU->CTRL = 0;

    TZ_SAU_Setup();

    /* Enable SAU */
    SAU->CTRL = SAU_CTRL_ENABLE_Msk;
}

/* Configure SoC security partitioning for secure/non-secure memory regions */
void soc_configure()
{
    /* Set non-secure vector table and stack pointer */
    DL_GSC_setVTOR_NS(GSC, NON_SECURE_APP0_START);
    __TZ_set_MSP_NS(NON_SECURE_APP0_SRAM_START + NON_SECURE_APP0_SRAM_LEN);
    /* Allow access to SYSCTL registers from Non Secure */
    DL_GSC_allowSYSCTLNonSecureAccess(GSC);

    /* Demonstrating configuration of Flash Security GSC attrbiute via Driverlib*/
    DL_GSC_configureMainFlashSecurity(
        GSC, DL_GSC_MAIN_FLASH_BANK_A, 0xFFFFFFFF, 0x00000000);
    /* SRAM's GSC security attrbiutes are configured using SYSCONFIG*/

    configure_sau();

    /* Memory barriers to ensure configuration is complete */
    __ISB();
    __DSB();
}

/* Test 1: Secure code accessing secure memory (should succeed) */
void test_secure_to_secure_access(void)
{
    /* Turn on secure LED to indicate secure memory access */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Read from secure buffer */
    volatile uint32_t value = secure_data_buffer[0];

    /* Write to secure buffer */
    secure_data_buffer[0] = value + 1;

    /* Turn off secure LED */
    DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

/* Test 2: Secure code accessing non-secure memory (should succeed) */
void test_secure_to_ns_memory_access(void)
{
    /* Turn on transition LED to indicate cross-domain access */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);

    /* Secure code can read from non-secure SRAM */
    volatile uint32_t value = *ns_sram_ptr;

    /* Secure code can write to non-secure SRAM */
    *ns_sram_ptr = 0xCAFEBABE;

    /* Turn off transition LED */
    DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(TEST_DELAY_CYCLES);
}

int main(void)
{
    /* Initialize secure world peripherals and GPIO */
    SYSCFG_DL_init();
    NVIC_EnableIRQ(BUTTON_GRP_INT_IRQN);

    /* Configure memory partitioning and security attributes */
    soc_configure();

    /* Run secure world tests - Valid Access Patterns */

    /* Test 1: Secure accessing secure memory (should succeed) */
    test_secure_to_secure_access();

    /* Test 2: Secure accessing non-secure memory (should succeed) */
    test_secure_to_ns_memory_access();

    /* Transfer button's interrupt to non-secure state via NVIC_ITNS register
     * Non-secure interrupts cannot pre-empt secure execution */
    volatile uint32_t state = NVIC_SetTargetState(BUTTON_GRP_INT_IRQN);
    state                   = NVIC_GetTargetState(BUTTON_GRP_INT_IRQN);

    /* Get non-secure reset handler */
    const uint32_t *p_ns_vector_table = (uint32_t *) NON_SECURE_APP0_START;
    const uint32_t *p_ns_reset_address =
        (const uint32_t *) ((uint32_t) NON_SECURE_APP0_START +
                            sizeof(uint32_t));
    nonsecure_func_t p_ns_reset = (nonsecure_func_t)(*p_ns_reset_address);

    /* Branch to non-secure application */
    p_ns_reset();

    while (1) {
    }
}

/* Custom HardFault handler, which turns on GREEN, RED and BLUE LEDs to indicate
* a failure state. It then enters an infinite loop, preserving the system state
* for examination by a debugger.
*/
void HardFault_Handler(void)
{
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    DL_GPIO_setPins(LED_GRP_TRANSITION_LED_PORT, LED_GRP_TRANSITION_LED_PIN);
    while (1) {
    }
}
