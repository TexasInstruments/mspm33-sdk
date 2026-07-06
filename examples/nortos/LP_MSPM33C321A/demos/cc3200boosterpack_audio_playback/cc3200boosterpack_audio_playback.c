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

#include <ti/boosterpacks/CC3200_audio_booster/AudioCodec.h>
#include "sound.h"
#include "ti_msp_dl_config.h"
#define SAMPLE_RATE 44100

__STATIC_INLINE void DL_GPIO_inena_Function(
    uint32_t pincmIndex, uint32_t inena_function)
{
    IOMUX->SECCFG.PINCM[pincmIndex] |=
        (inena_function | IOMUX_PINCM_PIPD_ENABLE);
}
void I2S1_IRQHandler(void)
{
    switch (DL_I2S_getPendingInterrupt(I2S_0_INST)) {
        case DL_I2S_IIDX_DMA_DONE_TX:
            DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
            break;
        default:
            break;
    }
}

int main(void)
{
    SYSCFG_DL_init();
    /*
    * Custom function that enables readback of output pins for loopback
    * Recommended to loopback clock signals for better timing closure
    */
    DL_GPIO_inena_Function(GPIO_I2S_0_IOMUX_WCLK, IOMUX_PINCM_INENA_ENABLE);
    DL_GPIO_inena_Function(GPIO_I2S_0_IOMUX_BCLK, IOMUX_PINCM_INENA_ENABLE);
    uint8_t status = AudioCodec_STATUS_SUCCESS;
    /*Reset Codec*/
    AudioCodec_reset(AudioCodec_TI_3254);
    /* Configure Codec */
    status =
        AudioCodec_config(AudioCodec_TI_3254, AudioCodec_16_BIT, SAMPLE_RATE,
            AudioCodec_STEREO, AudioCodec_SPEAKER_HP, AudioCodec_MIC_NONE);

    if (AudioCodec_STATUS_SUCCESS != status) {
        /* Error Initializing codec */
        while (1) {
        }
    }
    delay_cycles(100000);
    NVIC_EnableIRQ(I2S1_INT_IRQn);
    /* Configure the DMA  */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &DATA[0]);
    DL_DMA_setDestAddr(
        DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(&I2S_0_INST->TXDATA));
    DL_DMA_setTransferSize(
        DMA0, DMA0_CH0_CHAN_ID, (sizeof(DATA) / sizeof(DATA[0])));
    /* Enable the DMA Channel  */
    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);
    /* Enable the DAI module to start transferring the data */
    DL_I2S_enable(I2S_0_INST);

    /* Configure volume level for specific audio out on a codec device */
    AudioCodec_speakerVolCtrl(AudioCodec_TI_3254, AudioCodec_SPEAKER_HP, 60);

    /* Unmute Headphone line */
    AudioCodec_speakerUnmute(AudioCodec_TI_3254, AudioCodec_SPEAKER_HP);

    while (1) {
        __WFI();
    }
    __BKPT(0);
    return 0;
}
