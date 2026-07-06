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
 * Number of samples to send from controller to target.
 */
#define I2S_TX_PACKET_SIZE (64)

/* Test data to be transmitted */
const uint16_t gTxPacket[I2S_TX_PACKET_SIZE] = {0x0000, 0x1111, 0x2222, 0x3333,
    0x4444, 0x5555, 0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
    0xDDDD, 0xEEEE, 0xFFFF, 0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555,
    0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE,
    0xFFFF, 0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777,
    0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x0000,
    0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 0x8888, 0x9999,
    0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF};

uint16_t gTxCount = 0;

/* Function to enable readback of output pins for loopback */
__STATIC_INLINE void DL_GPIO_inena_Function(
    uint32_t pincmIndex, uint32_t inena_function)
{
    IOMUX->SECCFG.PINCM[pincmIndex] |=
        (inena_function | IOMUX_PINCM_PIPD_ENABLE);
}
int main(void)
{
    /* Initialize all configured peripherals */
    SYSCFG_DL_init();
    /* clear TXFIFO */
    DL_I2S_clearTXFIFO(I2S_0_INST);
    /* Enable readback of output pins for loopback */
    DL_GPIO_inena_Function(GPIO_I2S_0_IOMUX_WCLK, IOMUX_PINCM_INENA_ENABLE);
    DL_GPIO_inena_Function(GPIO_I2S_0_IOMUX_BCLK, IOMUX_PINCM_INENA_ENABLE);

    /* Fill the TX FIFO with initial data */
    gTxCount =
        DL_I2S_fillTXFIFO16(I2S_0_INST, &gTxPacket[0], I2S_TX_PACKET_SIZE);

    /* Continue filling TX FIFO as it empties */
    while (gTxCount < I2S_TX_PACKET_SIZE) {
        /* Wait for space in TX FIFO */
        while (DL_I2S_isTXFIFOFull(I2S_0_INST)) {
            ;
        }
        /* Fill more data into TX FIFO */
        gTxCount += DL_I2S_fillTXFIFO16(
            I2S_0_INST, gTxPacket + gTxCount, I2S_TX_PACKET_SIZE - gTxCount);
    }

    /* Wait for all data to be transmitted */
    while (!DL_I2S_isTXFIFOEmpty(I2S_0_INST)) {
        /* Wait for TX FIFO to empty */
    }
    __BKPT(0);
    /* Enter low power mode */
    while (1) {
        __WFI();
    }
    return 0;
}
