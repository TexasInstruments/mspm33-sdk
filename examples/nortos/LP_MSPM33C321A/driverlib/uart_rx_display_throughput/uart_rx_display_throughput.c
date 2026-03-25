/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
/* Data received from UART */
#define UART_PACKET_SIZE 1024
uint8_t gTxPacket[UART_PACKET_SIZE];
volatile uint8_t gRxPacket[UART_PACKET_SIZE];

uint8_t gExpectedRxPacket[UART_PACKET_SIZE];
volatile bool rxDmaDone = false;

int main(void)
{
    for(uint32_t i = 0; i < UART_PACKET_SIZE; i ++){
        gExpectedRxPacket[i] = (i % 256);
    }
    SYSCFG_DL_init();
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    
    
    DL_UART_enableInterrupt(
        UART_0_INST, DL_UART_INTERRUPT_DMA_DONE_RX);
    DL_UART_enableDMAReceiveEvent(
        UART_0_INST, DL_UART_DMA_INTERRUPT_RX);


    while(1){
        DL_DMA_setSrcAddr(
            DMA0, DMA0_CH1_CHAN_ID, (uint32_t)(&UART_0_INST->uart->RXDATA));
        DL_DMA_setDestAddr(DMA0, DMA0_CH1_CHAN_ID, (uint32_t) &gRxPacket[0]);
        DL_DMA_setTransferSize(DMA0, DMA0_CH1_CHAN_ID, UART_PACKET_SIZE);
        DL_DMA_enableChannel(DMA0, DMA0_CH1_CHAN_ID);
        
        while(rxDmaDone == false){
            __WFI();
        }
        rxDmaDone = false;
    }
}

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_IIDX_DMA_DONE_RX:
            rxDmaDone = true;
            break;
        default:
            break;
    }    
}
