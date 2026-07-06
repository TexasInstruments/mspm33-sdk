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

uint8_t gWelcomeMsg[] = "\r\n==== MSPM33 Console Test ====\r\n";
uint8_t gTestingMsg[] = "> Testing...0\r\n";

volatile bool gConsoleTxTransmitted, gConsoleTxDMATransmitted, gTransmitReady;
uint32_t increment        = 0;
uint8_t gExpectedTransfer = 5;
uint8_t gActualTransfer   = 0;
#ifdef __SLATE_INTERNAL__
volatile bool gDUTConfigured  = false;
volatile bool gDUTSentTesting = false;
#endif

void UART_Console_write(const uint8_t *data, uint16_t size)
{
    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(data));
    DL_DMA_setDestAddr(
        DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(&UART_0_INST->uart->TXDATA));
    DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, size);

    DL_SYSCTL_disableSleepOnExit();

    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

    while (false == gConsoleTxDMATransmitted) {
        __WFE();
    }

    while (false == gConsoleTxTransmitted) {
        __WFE();
    }

    gConsoleTxTransmitted    = false;
    gConsoleTxDMATransmitted = false;
}

int main(void)
{
    gTransmitReady           = false;
    gConsoleTxTransmitted    = false;
    gConsoleTxDMATransmitted = false;
    increment                = 0;

    SYSCFG_DL_init();

    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    DL_TimerG_startCounter(TIMER_0_INST);

    /* Write welcome message */
    UART_Console_write(&gWelcomeMsg[0], sizeof(gWelcomeMsg));

#ifdef __SLATE_INTERNAL__
    /* Wait here so we know device is configured but before anything is sent */
    gDUTConfigured = true;
    __BKPT(0);

#endif

    while (gActualTransfer != gExpectedTransfer) {
        if (gTransmitReady == true) {
            /* Will output "Testing...0", "Testing...1", etc. and loop after 9 */
            gTransmitReady  = false;
            gTestingMsg[12] = '0' + increment;
            increment       = (increment + 1) % 10;
            UART_Console_write(&gTestingMsg[0], sizeof(gTestingMsg));
            gActualTransfer++;
#ifdef __SLATE_INTERNAL__
            /* Wait here so we know device sent a transaction and can verify */
            gDUTSentTesting = true;
            __BKPT(0);
            gDUTSentTesting = false;
#endif
        } else {
            __WFI();
        }
    }
    __BKPT(0);
}

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_IIDX_EOT_DONE:
            gConsoleTxTransmitted = true;
            break;
        case DL_UART_IIDX_DMA_DONE_TX:
            gConsoleTxDMATransmitted = true;
            break;
        default:
            break;
    }
}

void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
            gTransmitReady = true;
            break;
        default:
            break;
    }
}
