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

volatile bool gCheckSEQ1, gCheckSEQ2, gCheckPreempt;

void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_1)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
        gCheckSEQ1 = true;
    }
}

void HSADC0_INST_INT2_IRQHandler(void)
{
    if (DL_HSADC_getInterruptStatus(HSADC0_INST, DL_HSADC_INT_2)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_2);
        gCheckSEQ2 = true;
    }
}

int main(void)
{
    SYSCFG_DL_init();

    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);
    NVIC_EnableIRQ(HSADC0_INST_INT2_IRQN);

    gCheckSEQ1    = false;
    gCheckSEQ2    = false;
    gCheckPreempt = false;

    /* trigger sequencers*/
    DL_HSADC_triggerSequencerSoftwareForce(HSADC0_INST, DL_HSADC_SEQ_NUMBER2);
    DL_HSADC_triggerSequencerSoftwareForce(HSADC0_INST, DL_HSADC_SEQ_NUMBER1);

    /* wait for interrupt handler to set gCheckSEQ1 to true*/
    while (false == gCheckSEQ1) {
        __WFE();
    }

    if (false == gCheckSEQ2) {
        gCheckPreempt = true;
    }

    if (gCheckPreempt) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    DL_HSADC_PowerDown(HSADC0_INST);
    DL_HSADC_disablePower(HSADC0_INST);

    __BKPT(0);
}
