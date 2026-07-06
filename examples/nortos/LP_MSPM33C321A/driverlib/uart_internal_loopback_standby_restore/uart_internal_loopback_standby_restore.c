/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

bool gActivateSleepPolicy = false;
bool gTogglePolicy        = false;

void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
            gTogglePolicy = true;
            break;
        default:
            break;
    }
}

int main(void)
{
    uint8_t txData = 0, rxData = 0;
    SYSCFG_DL_init();

    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    DL_TimerG_startCounter(TIMER_0_INST);

    /* Set LEDS_USER_LED_2 to start */
    DL_GPIO_setPins(GPIO_LEDS2_PORT, GPIO_LEDS2_USER_LED_2_PIN);

    while (1) {
        if (gTogglePolicy == true) {
            gTogglePolicy        = false;
            gActivateSleepPolicy = !gActivateSleepPolicy;
        }

        while ((gActivateSleepPolicy == true) && (gTogglePolicy == false)) {
            /* Wait for UART_0_INST to enter idle state */
            while (DL_UART_isBusy(UART_0_INST) == true)
                ;

            /* Turn off LEDs indicate entering STANDBY */
            DL_GPIO_clearPins(GPIO_LEDS2_PORT, GPIO_LEDS2_USER_LED_1_PIN);
            DL_GPIO_clearPins(GPIO_LEDS2_PORT, GPIO_LEDS2_USER_LED_2_PIN);

            /*
             * This implementation shows how to restore UART registers using
             *   the DriverLib saveConfiguration and restoreConfiguration
             *   functions. This approach is recommended if the UART
             *   configuration is modified at runtime.
             * Alternatively, SYSCFG_DL_UART_init can be called after waking-up
             *   to restore the default configuration from SysConfig. This
             *   approach is more memory-efficient.
             */
            /* Save UART_0_INST configuration before going to STANDBY */
            DL_SYSCTL_disableEventOnPend();
            /* Go to STANDBY and wait for interrupts */
            __WFI();
            
            DL_SYSCTL_enableEventOnPend();

            /* Turn on LED to indicate device is awake */
            DL_GPIO_setPins(GPIO_LEDS2_PORT, GPIO_LEDS2_USER_LED_2_PIN);
        }

        while ((gActivateSleepPolicy == false) && (gTogglePolicy == false)) {
            /*
             * While device is awake, transmit and receive incrementing data
             * through internal loopback
             */
            DL_UART_transmitData(UART_0_INST, txData);
            rxData = DL_UART_receiveDataBlocking(UART_0_INST);
            delay_cycles(1000000);
            if (txData == rxData) {
                DL_GPIO_togglePins(GPIO_LEDS2_PORT, GPIO_LEDS2_USER_LED_1_PIN);
            }
            txData = ++rxData;
        }
    }
    __BKPT(0);
}
