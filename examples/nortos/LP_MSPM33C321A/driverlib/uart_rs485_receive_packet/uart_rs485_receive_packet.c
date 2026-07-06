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

/*
 * Number of bytes for UART packet size
 * The packet will be transmitted by the UART.
 * This example uses FIFOs with polling, and the maximum FIFO size is 4.
 * Refer to interrupt examples to handle larger packets.
 */
#define UART_PACKET_SIZE (4)

/* Data received from UART */
uint8_t rxPacket[UART_PACKET_SIZE];

/* Expected data sent by uart_rs485_send_packet */
const uint8_t gExpectedPacket[UART_PACKET_SIZE] = {'M', 'S', 'P', '!'};

int main(void)
{
    SYSCFG_DL_init();

    /*
     * Wait to receive the UART data
     * This loop expects UART_PACKET_SIZE bytes
     */
    for (uint8_t i = 0; i < UART_PACKET_SIZE; i++) {
        rxPacket[i] = DL_UART_receiveDataBlocking(UART_0_INST);
    }

    /* Verify received data matches expected packet */
    bool dataCorrect = true;
    for (uint8_t i = 0; i < UART_PACKET_SIZE; i++) {
        if (rxPacket[i] != gExpectedPacket[i]) {
            dataCorrect = false;
            break;
        }
    }

    /* If data correct, set PASS LED */
    if (dataCorrect) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
        delay_cycles(1000);
    }
    /* Incorrect data received, set FAIL LED */
    else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
        delay_cycles(1000);
    }
    __BKPT(0);
}
