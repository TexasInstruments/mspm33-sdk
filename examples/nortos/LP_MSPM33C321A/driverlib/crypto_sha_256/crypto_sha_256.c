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
#include <string.h>

static const uint32_t knownHash[DL_SHAW_DIGEST_SIZE_256_BYTES / 4] = {
    0x30674edd,
    0x76320952,
    0xe3a9c07e,
    0x4c9ce13f,
    0xd69943e2,
    0x62ffa4eb,
    0xc91330f1,
    0x87ef30ed,
};

/* Buffer to store the computed hash */
static uint32_t computedHash[DL_SHAW_DIGEST_SIZE_256_BYTES / 4];
volatile bool gCorrectResults                    = false;
volatile bool digestReady = false;
volatile bool dmaDone = false;

void SHAW_IRQHandler(void)
{
    NVIC_ClearPendingIRQ( SHAW_INT_IRQn );

    if (DL_SHAW_getOutputReadyInterruptStatus(SHAW_INST)) {

        DL_SHAW_clearOutputReadyInterrupt(SHAW_INST);

        digestReady = true;
    }

    DL_SHAW_disableOutputReadyInterrupt(SHAW_INST);
}

void DMA0_IRQHandler(void)
{
    NVIC_ClearPendingIRQ( DMA0_INT_IRQn );

    if( DL_DMA_getRawInterruptStatus(DMA0, DL_DMA_INTERRUPT_CHANNEL0)) {

        DL_DMA_clearInterruptStatus(DMA0, DMA0_CH0_CHAN_ID);

        dmaDone = true;
    }

    DL_DMA_disableInterrupt(DMA0, DL_DMA_INTERRUPT_CHANNEL0);

}

int main(void)
{
    static uint8_t message[8*1024]__attribute__((aligned(4)));
    memset(message, 'a', sizeof(message)); // All bytes are 'a'
    uint32_t messageLength = sizeof(message);
    uint_fast8_t i;    
    SYSCFG_DL_init();

    DL_SHAW_STATUS status;
    const uint8_t *currentPtr = message;

    NVIC_EnableIRQ(SHAW_INT_IRQn);
    NVIC_EnableIRQ(DMA0_INT_IRQn);

    /* Configure SHA-256 mode */
    status = DL_SHAW_setModeConfig(SHAW_INST,
        DL_SHAW_ALGORITHM_SHA_256,
        DL_SHAW_MODE_HASH
    );

    if(status == DL_SHAW_STATUS_SUCCESS){
        /* Set total data length */
        status = DL_SHAW_setDataLength(SHAW_INST, messageLength, DL_SHAW_MODE_HASH);
    }

    DL_SHAW_signalModeAndLengthAvailable(SHAW_INST);

    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)((void*)currentPtr));

    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, DL_SHAW_DATA_IN_ADDR(SHAW_INST));

    DL_DMA_setTransferSize(
           DMA0, DMA0_CH0_CHAN_ID, messageLength/DL_SHAW_WORD_SIZE_BYTES );

    // Configure DMA for word transfers
    DL_DMA_setDestWidth(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_WIDTH_WORD);
    DL_DMA_setSrcWidth(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_WIDTH_WORD);

    // Set increment modes explicitly
    DL_DMA_setSrcIncrement(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_ADDR_INCREMENT);
    DL_DMA_setDestIncrement(DMA0, DMA0_CH0_CHAN_ID, DL_DMA_ADDR_UNCHANGED);

    DL_DMA_setTrigger(DMA0, DMA0_CH0_CHAN_ID, SHAW_DMA_TRIGGER, DL_DMA_TRIGGER_TYPE_EXTERNAL);

    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

    /* Start DMA trigger from SHA */
    DL_SHAW_startDMA(SHAW_INST);

    while(!dmaDone){
        __WFE();  // Wait for interrupt
    }

    while (!digestReady)
    {
        __WFE();  // Wait for interrupt
    }

    if (status == DL_SHAW_STATUS_SUCCESS) {
        /* Read the digest */
        status = DL_SHAW_readDigest(SHAW_INST, computedHash,
                                   DL_SHAW_DIGEST_SIZE_256_BYTES);
        if (status != DL_SHAW_STATUS_SUCCESS) {
            __BKPT(0);
        }

        /* Release the digest */
        DL_SHAW_releaseDigest(SHAW_INST);
    }
    
    gCorrectResults = true;
    for(i = 0; i<(DL_SHAW_DIGEST_SIZE_256_BYTES / 4); i++){
        gCorrectResults &= (computedHash[i]==knownHash[i]);
    }

    /* Verify the computed hash matches known value */
    if (!gCorrectResults) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    }

    __BKPT(0);

    return 0;
}
