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

/*
 * This example demonstrates how to use the I2S module in target mode with FIFO polling.
 * The target receives data from the controller through AD0 pin and sends it back through AD1 pin.
 * This example is designed to work with the i2s_controller_fifo_poll example in an external loopback
 * configuration.
 */

#include "ti_msp_dl_config.h"

/*
 * Number of samples to receive from controller and send back.
 * This example uses FIFO with polling. Refer to interrupt examples to handle larger packets.
 */
#define I2S_RX_PACKET_SIZE (64)
#define I2S_TX_PACKET_SIZE (64)

/* Buffer to store received data */
volatile uint16_t gRxPacket[I2S_RX_PACKET_SIZE];

/* Expected data to be received from controller */
const uint16_t gExpectedRxPacket[I2S_RX_PACKET_SIZE] = {0x0000, 0x1111, 0x2222,
    0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB,
    0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0x0000, 0x1111, 0x2222, 0x3333, 0x4444,
    0x5555, 0x6666, 0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD,
    0xEEEE, 0xFFFF, 0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666,
    0x7777, 0x8888, 0x9999, 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF,
    0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666, 0x7777, 0x8888,
    0x9999, 0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF};

uint16_t gRxCount = 0;

int main(void)
{
    /* Initialize all configured peripherals */
    SYSCFG_DL_init();
    DL_I2S_clearRXFIFO(I2S_0_INST);
    /* Wait for data from the Controller */
    for (gRxCount = 0; gRxCount < I2S_RX_PACKET_SIZE; gRxCount++) {
        while (DL_I2S_isRXFIFOEmpty(I2S_0_INST)) {
            ;
        }
        /* Read data from RX FIFO */
        gRxPacket[gRxCount] = DL_I2S_receiveData16(I2S_0_INST);
    }
    int allCorrect = 1;
    for (int i = 0; i < I2S_RX_PACKET_SIZE; i++) {
        if (gExpectedRxPacket[i] != gRxPacket[i]) allCorrect = 0;
    }

    if (allCorrect == 1) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    __BKPT(0);
    /* Enter low power mode */
    while (1) {
        __WFI();
    }
    return 0;
}
