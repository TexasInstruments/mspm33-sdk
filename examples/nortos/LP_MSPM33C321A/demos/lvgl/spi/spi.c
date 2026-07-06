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

#include "spi.h"
#include "../porting/lv_port_disp.h"

extern lv_disp_drv_t *pending_disp_drv;

/** Flag indicating SPI transmission is complete */
volatile bool gSPIDataTransmitted;

/** Flag indicating DMA transfer to SPI TX buffer is complete */
volatile bool gDMATXDataTransferred;

void SPI_init_flag()
{
    gSPIDataTransmitted = true;
}

void SPI_Send_DMA(const uint8_t *pBuffer, uint32_t size)
{
    /* Clear transmission flag */
    gSPIDataTransmitted = false;
    DL_GPIO_clearPins(LCD_CS_PORT, LCD_CS_PIN_0_PIN);
    // Add memory barrier
    __DSB(); // Data Synchronization Barrier

    /* Configure DMA source address (data to transmit) */
    DL_DMA_setSrcAddr(DMA0, DMA0_CH1_CHAN_ID, (uint32_t)pBuffer);

    /* Configure DMA destination address (SPI TX register) */
    DL_DMA_setDestAddr(DMA0, DMA0_CH1_CHAN_ID,
                       (uint32_t)(&SPI_0_INST->spi->TXDATA));

    /* Set number of 8-bit bytes to transfer */
    DL_DMA_setTransferSize(DMA0, DMA0_CH1_CHAN_ID, size);

    /* Enable DMA channel to start transfer */
    DL_DMA_enableChannel(DMA0, DMA0_CH1_CHAN_ID);

    // Add another barrier
    __DSB();

    /* Enable SPI interrupts for DMA completion and TX empty events */
    DL_SPI_enableInterrupt(SPI_0_INST,
                          (DL_SPI_INTERRUPT_DMA_DONE_TX |
                           DL_SPI_INTERRUPT_TX_EMPTY|DL_SPI_INTERRUPT_IDLE));

    /* Clear DMA transfer completion flag */
    gDMATXDataTransferred = false;
}

void SPI_0_INST_IRQHandler(void)
{
    uint32_t interruptStatus = DL_SPI_getPendingInterrupt(SPI_0_INST);
    switch (interruptStatus) 
    {
        /* DMA transfer to SPI TX buffer complete */
        case DL_SPI_IIDX_DMA_DONE_TX:
            gDMATXDataTransferred = true;
            DL_SPI_disableInterrupt(SPI_0_INST, DL_SPI_INTERRUPT_DMA_DONE_TX);
            DL_DMA_disableChannel(DMA0, DMA0_CH1_CHAN_ID);
            DL_GPIO_setPins(LCD_CS_PORT, LCD_CS_PIN_0_PIN);
            lv_port_disp_flush_ready();
            break;
        /* All data transmitted from SPI */
        case DL_SPI_IIDX_TX_EMPTY:
            gSPIDataTransmitted = true;
            DL_SPI_disableInterrupt(SPI_0_INST, DL_SPI_INTERRUPT_TX_EMPTY);
            break;
        default:
            break;
    }
}