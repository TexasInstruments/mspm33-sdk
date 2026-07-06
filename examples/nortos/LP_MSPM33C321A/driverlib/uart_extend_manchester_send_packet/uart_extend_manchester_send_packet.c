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

/* Number of bytes for UART packet size, the maximum FIFO size is 4 */
#define UART_PACKET_SIZE (4)

/* Delay for 5ms to ensure UART TX is idle before starting transmission */
#define UART_TX_DELAY (160000)

/* Data for UART to transmit */
uint8_t gTxPacket[UART_PACKET_SIZE] = {'M', 'S', 'P', '!'};

/* Data received by UART */
uint8_t gRxPacket[UART_PACKET_SIZE] = {0};
uint8_t allCorrect                  = 1;

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

int main(void)
{
    SYSCFG_DL_init();

    /* Optional delay to ensure UART TX is idle before starting transmission */
    delay_cycles(UART_TX_DELAY);

    /* Transmit data, then go to sleep while waiting for the echo back */
    DL_UART_fillTXFIFO(UART_0_INST, &gTxPacket[0], UART_PACKET_SIZE);

    /*
     * Wait until all bytes have been transmitted and the TX FIFO
     * is empty
     */
    while (DL_UART_isBusy(UART_0_INST))
        ;

    /* Wait to receive UART_PACKET_SIZE bytes of data */
    for (int i = 0; i < UART_PACKET_SIZE; i++) {
        gRxPacket[i] = DL_UART_receiveDataBlocking(UART_0_INST);
    }

    allCorrect =
        allCorrect && Comparison(gRxPacket, gTxPacket, UART_PACKET_SIZE);

    if (allCorrect == 1) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } 
    else{
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    __BKPT(0);
}
