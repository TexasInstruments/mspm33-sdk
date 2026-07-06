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

/*
 * Number of bytes for SPI packet size
 * The packet will be transmitted by the SPI peripheral.
 * This example uses FIFOs, and the maximum FIFO size is 4.
 */
#define SPI_PACKET_SIZE (4)

/* Data for SPI to transmit */
uint8_t gTxPacket[SPI_PACKET_SIZE] = {0x01, 0x02, 0x03, 0x04};

/* Data for SPI to receive */
uint8_t gRxPacket[SPI_PACKET_SIZE], gExpectedRxPacket[SPI_PACKET_SIZE] = {'M', 'S', 'P', '0'};

volatile bool gSPIDataTransmitted, gDMARXDataTransferred,
    gDMATXDataTransferred, gReceiveReady;
#ifdef __SLATE_INTERNAL__
#define TARGET_TX_READY 1
#define TARGET_RX_COMPLETE 2
volatile uint8_t gTestFlow = 0;
#endif
void SPI_receive(void);

int main(void)
{
    SYSCFG_DL_init();
    DL_SYSCTL_disableSleepOnExit();
    NVIC_EnableIRQ(SPI_0_INST_INT_IRQN);

    gReceiveReady         = true;
    gSPIDataTransmitted   = false;
    gDMARXDataTransferred = false;

    while (1) {
        if (gReceiveReady == true) {
            /*
             * Fill FIFO with data.
             * Note that transactions are initiated by the Controller, so this function
             * only fills the buffer and the Peripheral device will send this data when
             * requested by the Controller.
             */
            DL_SPI_fillTXFIFO8(SPI_0_INST, &gTxPacket[0], SPI_PACKET_SIZE);
#ifdef __SLATE_INTERNAL__
            gTestFlow = TARGET_TX_READY;
            /* This breakpoint is needed to trigger ioth to start transaction */
            __BKPT(0);
#endif
            gReceiveReady = false;
            SPI_receive();
        } else {
            __WFI();
        }
    }
}

void SPI_receive(void)
{
    /*
     * Configure DMA0 source, destination and size from RXDATA to gRxPacket.
     * The DMA0 transfer will start when the RX interrupt is set, which happens
     * when the device receives data.
     */
    DL_DMA_setSrcAddr(
        DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(&SPI_0_INST->spi->RXDATA));
    DL_DMA_setDestAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gRxPacket[0]);
    DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, SPI_PACKET_SIZE);

    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

    DL_SPI_enableInterrupt(SPI_0_INST, DL_SPI_INTERRUPT_TX_EMPTY);

    while (false == gSPIDataTransmitted) {
        __WFE();
    }

    while (false == gDMARXDataTransferred) {
        __WFE();
    }
    /*
     * Optional SW breakpoint to check results.
     * If this example is used with the
     * spi_controller_repeated_fifo_dma_interrupts example,
     * the expected data to receive in gRxPacket is
     * {'M', 'S', 'P', 'x'}, where 'x' starts at '0' and
     * should increment every time the Controller example
     * sends a new data packet.
     */
    //    __BKPT(0);
    gSPIDataTransmitted   = false;
    gDMARXDataTransferred = false;
    gReceiveReady         = true;
#ifdef __SLATE_INTERNAL__
    gTestFlow = TARGET_RX_COMPLETE;
    /* Break point is needed here else target will increase gTxPacket value below and
        *  the test would fail in slate
        */
    __BKPT(0);
#endif

    uint8_t allCorrect = 1;
    for(uint8_t i = 0; i < SPI_PACKET_SIZE; i ++){
        if(gExpectedRxPacket[i] != gRxPacket[i]){
            allCorrect = 0;
            break;
        }
    }

    if (allCorrect == 1) {
        DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } 
    else{
        DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    /* Increment data to send */
    gTxPacket[0]++;
    gExpectedRxPacket[3]++;
}

void SPI_0_INST_IRQHandler(void)
{
    switch (DL_SPI_getPendingInterrupt(SPI_0_INST)) {
        case DL_SPI_IIDX_TX_EMPTY:
            /* SPI is done transmitting data and TXFIFO is empty */
            gSPIDataTransmitted = true;
            DL_SPI_disableInterrupt(SPI_0_INST, DL_SPI_INTERRUPT_TX_EMPTY);
            break;
        case DL_SPI_IIDX_DMA_DONE_RX:
            /* DMA0 is done transferring data from RXFIFO to gRxPacket*/
            gDMARXDataTransferred = true;
            break;

        default:
            break;
    }
}
