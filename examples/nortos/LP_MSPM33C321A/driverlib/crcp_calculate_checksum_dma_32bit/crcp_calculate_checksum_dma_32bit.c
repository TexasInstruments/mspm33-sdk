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

#define CRCP_NUM_OF_WORDS (3)
#define CRCP_EXPECTED_RESULT (0x91A5D23B)

uint32_t gData32[CRCP_NUM_OF_WORDS] = {0x3B4A5812, 0x0C74FECA, 0x0000842F};
const uint32_t gCrcpSeed            = CRCP_SEED;
volatile bool gCheckCRCP;
void DMA0_IRQHandler(void)
{
    switch (DL_DMA_getPendingInterrupt(DMA0)) {
        case DL_DMA_EVENT_IIDX_DMACH0:
            gCheckCRCP = true;
            break;
        default:
            break;
    }
}

int main(void)
{
    volatile uint32_t crcpChecksum;
    __attribute__((unused)) volatile bool crcpChecksumMatch;

    SYSCFG_DL_init();

    /* Configure DMA source, destination and size */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gData32[0]);
    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, DL_CRCP_getCRCINAddr(CRCP0));
    DL_DMA_setTransferSize(
        DMA0, DMA0_CH0_CHAN_ID, sizeof(gData32) / sizeof(uint32_t));
    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

    /*
     * Confirm DMA channel is enabled before issuing a SW trigger of the DMA
     * channel
     */
    while (false == DL_DMA_isChannelEnabled(DMA0, DMA0_CH0_CHAN_ID)) {
        __BKPT(0);
    }

    /* Configure device exception handling */
    DL_SYSCTL_disableSleepOnExit();
    NVIC_EnableIRQ(DMA0_INT_IRQn);
    gCheckCRCP = false;

    /* Trigger DMA via SW request */
    DL_DMA_startTransfer(DMA0, DMA0_CH0_CHAN_ID);

    /* Wait in SLEEP mode until DMA interrupt is triggered */
    while (false == gCheckCRCP) {
        __WFE();
    }

    /* Check CRCP checksum */
    crcpChecksum = DL_CRCP_getResult32(CRCP0);

    if (CRCP_EXPECTED_RESULT == crcpChecksum) {
        crcpChecksumMatch = true;
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        crcpChecksumMatch = false;
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    __BKPT(0);

    while (1) {
        __WFI();
    }
}
