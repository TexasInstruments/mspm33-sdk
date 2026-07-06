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
#define gTestingMsgType 1
#define gWelcomeMsgType 0
uint8_t gExpectedWelcomeMsg[] = "\r\n==== MSPM33 Console Test ====\r\n";
uint8_t gExpectedTestingMsg[] = "> Testing...0\r\n";

uint8_t gTestingMsg[50];
uint8_t gExpectedTransfer = 5;
uint8_t gActualTransfer   = 0;

volatile bool gDMARXDataTransferred;

uint32_t increment = 0;
void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_IIDX_DMA_DONE_RX:
            /* DMA0 is done transferring data from RXFIFO to gRxPacket*/
            gDMARXDataTransferred = true;
            break;

        default:
            break;
    }
}

uint8_t Comparison(uint8_t *data1, uint8_t *data2, uint8_t dataLength)
{
    uint8_t equal = 1;
    for (int i = 0; i < dataLength; i++) {
        if (data1[i] != data2[i]) {
            equal = 0;
            break;
        }
    }
    return equal;
}

uint8_t UART_Console_read(uint16_t size, uint8_t msgType)
{
    uint8_t correct = 1;
    /*
     * Configure DMA0 source, destination and size from RXDATA to gRxPacket.
     * The DMA0 transfer will start when the RX interrupt is set, which happens
     * when the device receives data.
     */
    DL_DMA_setSrcAddr(
        DMA0, DMA0_CH1_CHAN_ID, (uint32_t)(&UART_0_INST->uart->RXDATA));
    DL_DMA_setDestAddr(DMA0, DMA0_CH1_CHAN_ID, (uint32_t) &gTestingMsg[0]);
    DL_DMA_setTransferSize(DMA0, DMA0_CH1_CHAN_ID, size);

    DL_DMA_enableChannel(DMA0, DMA0_CH1_CHAN_ID);

    DL_SYSCTL_disableSleepOnExit();
#ifdef __SLATE_INTERNAL__
    __BKPT(0);
#endif

    while (false == gDMARXDataTransferred) {
        __WFI();
    }

    if (msgType == gWelcomeMsgType) {
        if (Comparison(gExpectedWelcomeMsg, gTestingMsg, size) == 0) {
            correct = 0;
        }
    } else if (msgType == gTestingMsgType) {
        gExpectedTestingMsg[12] = '0' + increment;
        increment               = (increment + 1) % 10;

        if (Comparison(gExpectedTestingMsg, gTestingMsg, size) == 1) {
            gActualTransfer++;
        } else {
            correct = 0;
        }
    }

    gDMARXDataTransferred = false;
    return correct;
}

int main(void)
{
    gDMARXDataTransferred = false;
    uint8_t allCorrect          = 1;
    SYSCFG_DL_init();

    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    /* Enable UART RX interrupt as a trigger for DMA0 */
    DL_UART_enableDMAReceiveEvent(
        UART_0_INST, DL_UART_DMA_INTERRUPT_RX);

    DL_UART_enableInterrupt(
        UART_0_INST, DL_UART_INTERRUPT_DMA_DONE_RX);

    allCorrect = UART_Console_read(sizeof(gExpectedWelcomeMsg), gWelcomeMsgType);
    if (allCorrect == 1) {
        while (gExpectedTransfer != gActualTransfer) {
            allCorrect = UART_Console_read(
                sizeof(gExpectedTestingMsg), gTestingMsgType);
            if (allCorrect == 0) break;
        }
    }

    if (allCorrect == 1) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } 
    else{
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    __BKPT(0);
}
