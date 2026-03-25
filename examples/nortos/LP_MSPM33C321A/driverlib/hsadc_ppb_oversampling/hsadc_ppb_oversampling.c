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
#include <stdio.h>
#include "uart_console.h"

#define ADC_OVERSAMPLE_COUNT 8U
#define ADC_MAX_RESULT      0xFFFU
#define ADC_VREF_VOLTAGE_mV 3300U

volatile bool gCheckADC1, gCheckADC2, gCheckADC3;
volatile uint16_t adcResult1, adcResult2, adcResult3;
volatile uint16_t adcResult1mv, adcResult2mv, adcResult3mv;

void HSADC0_INST_INT1_IRQHandler(void)
{
    if (DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST, DL_HSADC_INT_1)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_1);
        gCheckADC1 = true;
    }
}

void HSADC0_INST_INT2_IRQHandler(void)
{
    if (DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST, DL_HSADC_INT_2)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_2);
        gCheckADC2 = true;
    }
}

void HSADC0_INST_INT3_IRQHandler(void)
{
    if (DL_HSADC_getInterruptResultReadyStatus(HSADC0_INST, DL_HSADC_INT_3)) {
        DL_HSADC_InterruptStatusClear(HSADC0_INST, DL_HSADC_INT_3);
        gCheckADC3 = true;
    }
}

int main(void)
{
    SYSCFG_DL_init();

    NVIC_EnableIRQ(HSADC0_INST_INT1_IRQN);
    NVIC_EnableIRQ(HSADC0_INST_INT2_IRQN);
    NVIC_EnableIRQ(HSADC0_INST_INT3_IRQN);

    /* trigger the sequencer*/
    DL_HSADC_triggerSequencerSoftwareForce(HSADC0_INST, DL_HSADC_SEQ_NUMBER1);
    DL_HSADC_triggerSequencerSoftwareForce(HSADC0_INST, DL_HSADC_SEQ_NUMBER2);
    DL_HSADC_triggerSequencerSoftwareForce(HSADC0_INST, DL_HSADC_SEQ_NUMBER3);

    /* wait till all samples are done*/
    while ((false == gCheckADC1) || (false == gCheckADC2) || (false == gCheckADC3))
        ;

    adcResult1 =
        DL_HSADC_getFinalSumResult(HSADC0_RESULT_INST, DL_HSADC_SEQ_NUMBER1);

    adcResult1mv = 
        (uint16_t)((adcResult1*ADC_VREF_VOLTAGE_mV)/ADC_MAX_RESULT);

    adcResult2 =
        DL_HSADC_getFinalSumResult(HSADC0_RESULT_INST, DL_HSADC_SEQ_NUMBER2) / ADC_OVERSAMPLE_COUNT;

    adcResult2mv = 
        (uint16_t)((adcResult2*ADC_VREF_VOLTAGE_mV)/ADC_MAX_RESULT);
        
    adcResult3 =
        DL_HSADC_getFinalSumResult(HSADC0_RESULT_INST, DL_HSADC_SEQ_NUMBER3);

    adcResult3mv = 
        (uint16_t)((adcResult1*ADC_VREF_VOLTAGE_mV)/ADC_MAX_RESULT);

    uart_printf(UART_0_INST,"Sequencer 1 (without oversampling - 12 bit result) : %d, %d mV \n", adcResult1, adcResult1mv);
    uart_printf(UART_0_INST,"Sequencer 2 (with oversampling - 15 bit result) : %d, %d mV \n", adcResult2, adcResult2mv);
    uart_printf(UART_0_INST,"Sequencer 3 (with oversampling and right shift by 3 - 12 bit result) : %d, %d mV \n", adcResult3, adcResult3mv);
    __BKPT(0);
}
