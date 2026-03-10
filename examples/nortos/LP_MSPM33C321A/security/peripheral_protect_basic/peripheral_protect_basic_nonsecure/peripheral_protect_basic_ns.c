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

#include "nsc_example.h"
#include "ti_msp_dl_config.h"

/* Delay constants for visual feedback timing */
#define TRANSITION_DELAY_CYCLES 4000000
#define TEST_DELAY_CYCLES 2000000

/* Button press flag */
volatile bool buttonPressed = false;

/* Forward declarations */
void test_nonsecure_gpio_access(void);
void test_secure_gpio_access_from_nonsecure(void);

/**
 * @brief Test non-secure GPIO access from non-secure world
 *
 * This function demonstrates successful access to a non-secure peripheral (GPIOB)
 * from non-secure code. The RED LED will blink to indicate success.
 */
void test_nonsecure_gpio_access(void)
{
    /* Blink RED LED (non-secure GPIO) - should succeed */
    for (int i = 0; i < 3; i++) {
        DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
        delay_cycles(TEST_DELAY_CYCLES);
        DL_GPIO_clearPins(
            LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
        delay_cycles(TEST_DELAY_CYCLES);
    }

    /* Keep RED LED on to indicate non-secure GPIO access successful */
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
}

/**
 * @brief Attempt to access secure GPIO from non-secure world
 *
 * This function attempts to access a secure peripheral (GPIOA) from non-secure code.
 * This should trigger a security violation and cause a fault.
 *
 * NOTE: This function is intentionally designed to fail as a demonstration
 * of peripheral protection. If peripheral protection is working correctly,
 * this will trigger a fault and all LEDs will turn on.
 */
void test_secure_gpio_access_from_nonsecure(void)
{
    /* WARNING: This will cause a security violation!
     * Attempting to access secure GPIOA (GREEN LED) from non-secure code */

    /* This access should trigger a security violation */
    DL_GPIO_setPins(LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
}

int main(void)
{
    /* Test 2: Non-secure code accesses non-secure GPIO (GPIOB) → Should succeed (RED LED blinks) */
    test_nonsecure_gpio_access();

    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* Wait for button press to proceed with violation test */
    while (!buttonPressed) {
        __WFI();
    }

    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* Clear RED LED before attempting secure access */
    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);

    /* Test 3: Non-secure code attempts to access secure GPIO (GPIOA) → Should fault (ALL LEDs)
     *
     * IMPORTANT: This is an intentional security violation test!
     * When peripheral protection is working correctly, this will trigger a fault
     * and the NMI handler will turn on all LEDs to indicate the violation.
     *
     * Comment out the line below to skip the violation test. */
    test_secure_gpio_access_from_nonsecure();

    /* If we reach here, peripheral protection is NOT working correctly
     * (the secure GPIO access should have caused a fault) */
    DL_GPIO_setPins(LED_GRP_TRANSITION_LED_PORT, LED_GRP_TRANSITION_LED_PIN);

    /* Infinite loop */
    while (1) {
        __WFI();
    }
}

/**
 * @brief Non-secure interrupt handler - handles button press in non-secure world
 *
 * This interrupt handler is configured to run in non-secure state.
 * It sets a flag when the button is pressed, allowing the main loop to proceed
 * with the security violation test.
 */
void BUTTON_GRP_INT_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(BUTTON_GRP_PORT)) {
        case BUTTON_GRP_USER_SWITCH_IIDX:
            /* If button is pressed (high), set flag to proceed with test */
            if (!DL_GPIO_readPins(
                    BUTTON_GRP_PORT, BUTTON_GRP_USER_SWITCH_PIN)) {
                DL_GPIO_clearInterruptStatus(
                    BUTTON_GRP_PORT, BUTTON_GRP_USER_SWITCH_PIN);
                buttonPressed = true;
            }
            break;

        default:
            break;
    }
}
