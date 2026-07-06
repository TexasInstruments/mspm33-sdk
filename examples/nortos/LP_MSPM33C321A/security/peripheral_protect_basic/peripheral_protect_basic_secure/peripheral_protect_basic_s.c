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

/* Delay constants for visual feedback timing */
#define TRANSITION_DELAY_CYCLES 4000000
#define TEST_DELAY_CYCLES 2000000

/* Forward declarations */
void configure_sau(void);
void soc_configure(void);
void configure_peripheral_security(void);
void test_secure_gpio_access(void);

/* Configure Security Attribution Unit (SAU) for memory region security */
void configure_sau(void)
{
    /* Disable SAU */
    SAU->CTRL = 0;

    /* Setup SAU regions for secure/non-secure memory partitioning */
    TZ_SAU_Setup();

    /* Enable SAU */
    SAU->CTRL = SAU_CTRL_ENABLE_Msk;
}

/* Configure SoC security partitioning for secure/non-secure memory regions */
void soc_configure(void)
{
    /* Set non-secure vector table and stack pointer */
    DL_GSC_setVTOR_NS(GSC, NON_SECURE_APP0_START);
    __TZ_set_MSP_NS(NON_SECURE_APP0_SRAM_START + NON_SECURE_APP0_SRAM_LEN);
    /* Allow access to SYSCTL registers from Non Secure*/
    DL_GSC_allowSYSCTLNonSecureAccess(GSC);

    // GSC->SPC_SECATTRIB2 = APPS_GSC_PROPS_SRAM_BANK2;
    // GSC->SPC_SECATTRIB3 = APPS_GSC_PROPS_SRAM_BANK3;

    /* Demonstrating configuration of Flash Security GSC attrbiute via Driverlib*/
    DL_GSC_configureMainFlashSecurity(
        GSC, DL_GSC_MAIN_FLASH_BANK_A, 0xFFFFFFFF, 0x00000000);
    /* SRAM's GSC security attrbiutes are configured using SYSCONFIG*/

    /* Configure Flash Bank1 */
    // GSC->FPC_SECATTRIBA1 = APPS_GSC_PROPS_FLASH_BANK_A1;
    // GSC->FPC_SECATTRIBB1 = APPS_GSC_PROPS_FLASH_BANK_B1;

    /* Configure SAU regions */
    configure_sau();

    /* Memory barriers to ensure configuration is complete */
    __ISB();
    __DSB();
}

/**
 * @brief Test secure GPIO access from secure world
 *
 * This function demonstrates successful access to a secure peripheral (GPIOA)
 * from secure code. The GREEN LED will blink to indicate success.
 */
void test_secure_gpio_access(void)
{
    /* Blink GREEN LED (secure GPIO) to show secure access works */
    for (int i = 0; i < 3; i++) {
        DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
        delay_cycles(TEST_DELAY_CYCLES);
        DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
        delay_cycles(TEST_DELAY_CYCLES);
    }

    /* Leave GREEN LED on to indicate secure GPIO access successful */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
}

int main(void)
{
    /* Initialize peripherals via SysConfig */
    SYSCFG_DL_init();

    /* Turn on secure LED briefly to indicate program is running in Secure world */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    delay_cycles(TRANSITION_DELAY_CYCLES);
    DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);

    /* Configure memory partitioning and security attributes */
    soc_configure();

    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* Test 1: Secure code accesses secure GPIO (GPIOA) → Should succeed (GREEN LED blinks) */
    test_secure_gpio_access();

    delay_cycles(TRANSITION_DELAY_CYCLES);
    DL_GPIO_clearPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);

    /* Enable button interrupt for non-secure world */
    NVIC_EnableIRQ(BUTTON_GRP_INT_IRQN);

    /* Transfer button's interrupt to non-secure state via NVIC_ITNS register
     * Non-secure interrupts cannot pre-empt secure execution */
    volatile uint32_t state = NVIC_SetTargetState(BUTTON_GRP_INT_IRQN);
    state                   = NVIC_GetTargetState(BUTTON_GRP_INT_IRQN);

    /* Get non-secure reset handler from non-secure vector table */
    const uint32_t* p_ns_vector_table = (uint32_t*) NON_SECURE_APP0_START;
    const uint32_t* p_ns_reset_address =
        (const uint32_t*) ((uint32_t) NON_SECURE_APP0_START +
                           sizeof(uint32_t));
    nonsecure_func_t p_ns_reset = (nonsecure_func_t)(*p_ns_reset_address);

    /* Branch to non-secure application
     * Non-secure code will test:
     * - Access to non-secure GPIO (GPIOB) (should succeed - RED LED)
     * - Attempt to access secure GPIO (GPIOA) (should fault - ALL LEDs) */
    p_ns_reset();

    /* Should never reach here */
    while (1) {
        __WFI();
    }
}

/**
 * @brief NMI Handler - catches non-maskable interrupts and security violations
 *
 * This handler is invoked when a security violation occurs, such as:
 * - Non-secure code attempting to access secure peripherals
 * - Secure code attempting to access non-secure peripherals (if blocked)
 * - Invalid memory access across security boundaries
 *
 * Visual feedback: ALL LEDs turn ON to indicate fault condition
 */
void NMI_Handler(void)
{
    /* Turn on all LEDs to indicate fault condition */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
    DL_GPIO_setPins(LED_GRP_TRANSITION_LED_PORT, LED_GRP_TRANSITION_LED_PIN);

    /* Halt system */
    while (1) {
        __WFI();
    }
}
