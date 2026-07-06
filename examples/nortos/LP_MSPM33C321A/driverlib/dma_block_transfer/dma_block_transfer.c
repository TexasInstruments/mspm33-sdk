/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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

#define DMA_TRANSFER_SIZE_WORDS (16)

const uint32_t gSrcData[DMA_TRANSFER_SIZE_WORDS] = {0x00000000, 0x10101010,
    0x20202020, 0x30303030, 0x40404040, 0x50505050, 0x60606060, 0x70707070,
    0x80808080, 0x90909090, 0xA0A0A0A0, 0xB0B0B0B0, 0xC0C0C0C0, 0xD0D0D0D0,
    0xE0E0E0E0, 0xF0F0F0F0};
uint32_t gDstData[DMA_TRANSFER_SIZE_WORDS];
volatile bool gChannel0InterruptTaken = false;
volatile bool gVerifyResult           = false;

#ifdef __SLATE_INTERNAL__
volatile bool gTestComplete = false;
#endif

void DMA0_IRQHandler(void)
{
    /* Example interrupt code -- just used to break the WFI in this example */
    switch (DL_DMA_getPendingInterrupt(DMA0)) {
        case DL_DMA_EVENT_IIDX_DMACH0:
            gChannel0InterruptTaken = true;
            break;
        default:
            break;
    }
}

int main(void)
{
    /*
     * Initialize the DMA peripheral and set up a transaction according to
     * the parameters defined in ti_msp_dl_config.h. The LED (USER_LED_1)
     * should be off.
     */
    SYSCFG_DL_init();

    /* Setup interrupts on device */
    DL_SYSCTL_disableSleepOnExit();
    NVIC_EnableIRQ(DMA0_INT_IRQn);
    // DL_GSC_PPC_setPeripheralSecurity(GSC,DL_GSC_PPC_ATTRIB_SECURE_DMA,1,DL_GSC_ATTRIBUTE_TYPE_SECURE);
    /* Configure DMA source, destination and size */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gSrcData[0]);
    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gDstData[0]);
    DL_DMA_setTransferSize(
        DMA0, DMA0_CH0_CHAN_ID, sizeof(gSrcData) / sizeof(uint32_t));
    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

    /* Start block transfer */
    gChannel0InterruptTaken = false;
    DL_DMA_startTransfer(DMA0, DMA0_CH0_CHAN_ID);

    /* Wait for transfer */
    while (gChannel0InterruptTaken == false) {
        __WFE();
    }

    gVerifyResult = true;
    for (int i = 0; i < DMA_TRANSFER_SIZE_WORDS; i++) {
        gVerifyResult &= gSrcData[i] == gDstData[i];
    }

    if (gVerifyResult == true) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    /* Breakpoint to inspect verification result */
    __BKPT(0);

    while (1) {
        __WFI();
    }
}
