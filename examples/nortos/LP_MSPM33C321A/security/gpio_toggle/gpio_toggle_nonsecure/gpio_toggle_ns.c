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

#include "nsc_example.h"

#define TRANSITION_DELAY_CYCLES 100000000
volatile bool buttonPressed = false;

int main(void)
{
    volatile uint32_t ii = 1;

    /* Initialize non-secure world peripherals and GPIO */
    SYSCFG_DL_init();
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);

    /* Wait for button press in non-secure world
     * Interrupt is handled by NS IRQ handler (configured in secure world) */
    while (!buttonPressed) {
        __WFI();
    }
    delay_cycles(TRANSITION_DELAY_CYCLES);

    /* Clear non-secure LED before calling secure function */
    DL_GPIO_clearPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);

    /* Call Secure function via Non Secure Callable(NSC) to access secure resources */
    hello_secure();

    /* Re-enable non-secure LED after returning from secure world */
    DL_GPIO_setPins(LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);

    while (ii) {
    }
}

/* Non-secure interrupt handler - handles button press in non-secure world */
void BUTTON_GRP_INT_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(BUTTON_GRP_PORT)) {
        case BUTTON_GRP_USER_SWITCH_IIDX:
            /* If SW is high, set flag to proceed with NSC call */
            if (!DL_GPIO_readPins(
                    BUTTON_GRP_PORT, BUTTON_GRP_USER_SWITCH_PIN)) {
                DL_GPIO_clearInterruptStatus(
                    BUTTON_GRP_PORT, BUTTON_GRP_USER_SWITCH_PIN);
                buttonPressed = true;
            }
            /* Otherwise, turn all LEDs ON to indicate failure state */
            else {
                DL_GPIO_setPins(
                    LED_GRP_SECURE_LED_PORT, LED_GRP_SECURE_LED_PIN);
                DL_GPIO_setPins(
                    LED_GRP_NONSECURE_LED_PORT, LED_GRP_NONSECURE_LED_PIN);
                DL_GPIO_setPins(
                    LED_GRP_TRANSITION_LED_PORT, LED_GRP_TRANSITION_LED_PIN);
            }
            break;

        default:
            break;
    }
}
