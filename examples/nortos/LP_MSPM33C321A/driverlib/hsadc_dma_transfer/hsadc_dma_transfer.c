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

#define ADC_SAMPLE_SIZE (1024)
/* When FIFO is enabled 2 samples are compacted in a single word */
#define ADC_FIFO_SAMPLES (ADC_SAMPLE_SIZE >> 1)

static uint32_t gADCSamples[ADC_FIFO_SAMPLES];
volatile bool gCheckADC;

void DMA0_IRQHandler(void)
{
    if (DL_HSADC_getDMAInterruptStatus(HSADC0_INST, DL_HSADC_DMA_INT_1)) {
        DL_HSADC_DMAInterruptStatusClear(HSADC0_INST, DL_HSADC_DMA_INT_1);
        gCheckADC = true;
    }
}

int main(void)
{
    SYSCFG_DL_init();

    /* Configure DMA source, destination and size */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID,
        (uint32_t) (&(
            HSADC0_RESULT_INST->ADC_LITE_RESULT_REGS.ADCSEQ1FIFORESULT)));

    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gADCSamples[0]);
    DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, ADC_FIFO_SAMPLES);

    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

    /* Setup interrupts on device */
    NVIC_EnableIRQ(DMA0_INT_IRQn);

    gCheckADC = false;

    while (false == gCheckADC) {
        DL_HSADC_triggerSequencerSoftwareForce(
            HSADC0_INST, DL_HSADC_SEQ_NUMBER1);

        while (DL_HSADC_isBusy(HSADC0_INST))
            ;
    }
    __BKPT(0);
}