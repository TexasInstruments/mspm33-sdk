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

/*
 * Monitoring voltage needs to be half of voltage reference
 * Since this is a 12 bit ADC, voltage input of 1.4V(vref) will result in 0xfff
 * Thus monitoring voltage = 0xfff/2 = 0x7ff
 */
#define HSADC_MONITOR_VALUE (0x7ff)

volatile bool gCheckADC;
volatile uint16_t gADCResult;

void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST, DL_HSADC_INT_1)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
        gCheckADC = true;
    }
}

int main(void)
{
    SYSCFG_DL_init();

    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);

    gCheckADC = false;

    /* Confirm VREF has settled before triggering HSADC conversion */
    while (DL_VREF_CTL1_READY_NOTRDY == DL_VREF_getStatus(VREF))
        ;

    while (1) {
        DL_HSADC_triggerSequencerSoftwareForce(
            HSADC0_INST, DL_HSADC_SEQ_NUMBER1);

        while (false == gCheckADC)
            ;

        gADCResult =
            DL_HSADC_getResult(HSADC0_RESULT_INST, DL_HSADC_SOC_NUMBER0);

        if (gADCResult > HSADC_MONITOR_VALUE) {
            DL_GPIO_clearPins(GPIO_LEDS_1_PORT, GPIO_LEDS_1_USER_LED_1_PIN);
        } else {
            DL_GPIO_setPins(GPIO_LEDS_1_PORT, GPIO_LEDS_1_USER_LED_1_PIN);
        }
        gCheckADC = false;
    }
}
