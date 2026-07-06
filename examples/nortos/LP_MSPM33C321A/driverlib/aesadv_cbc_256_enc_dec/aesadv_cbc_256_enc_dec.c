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

#ifdef __SLATE_INTERNAL__
volatile bool gTestComplete = false;
#endif

uint8_t g_plaintext[16] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};

uint8_t g_iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

uint8_t g_key[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b,
    0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b,
    0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};

const uint8_t g_goldenData[16] = {0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1,
    0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6};

volatile bool gCorrectResults = false;

// Interrupt status flags
volatile bool contextWriteable = false;
volatile bool inputReady       = false;
volatile bool outputReady      = false;

void AESADV_IRQHandler()
{
    // Read interrupt status
    uint32_t status = DL_AESADV_getRawInterruptStatus(
        AESADV_INST, DL_AESADV_INTERRUPT_INPUT_CONTEXT_READY |
                         DL_AESADV_INTERRUPT_INPUT_READY |
                         DL_AESADV_INTERRUPT_OUTPUT_READY);

    // Handle context ready interrupt
    if (status & DL_AESADV_INTERRUPT_INPUT_CONTEXT_READY) {
        contextWriteable = true;
        DL_AESADV_clearInterruptStatus(
            AESADV_INST, DL_AESADV_INTERRUPT_INPUT_CONTEXT_READY);
        DL_AESADV_disableInterrupt(
            AESADV_INST, DL_AESADV_INTERRUPT_INPUT_CONTEXT_READY);
    }

    // Handle input ready interrupt
    if (status & DL_AESADV_INTERRUPT_INPUT_READY) {
        inputReady = true;
        DL_AESADV_clearInterruptStatus(
            AESADV_INST, DL_AESADV_INTERRUPT_INPUT_READY);
        DL_AESADV_disableInterrupt(
            AESADV_INST, DL_AESADV_INTERRUPT_INPUT_READY);
    }

    // Handle output ready interrupt
    if (status & DL_AESADV_INTERRUPT_OUTPUT_READY) {
        outputReady = true;
        DL_AESADV_clearInterruptStatus(
            AESADV_INST, DL_AESADV_INTERRUPT_OUTPUT_READY);
        DL_AESADV_disableInterrupt(
            AESADV_INST, DL_AESADV_INTERRUPT_OUTPUT_READY);
    }
}

int main(void)
{
    uint8_t ciphertext[16];
    uint8_t buffer[16];
    uint_fast8_t i;
    DL_AESADV_Config *config = &gAESADV_config;

    config->iv = g_iv;

    /* Initialize system */
    SYSCFG_DL_init();

    DL_Interrupt_registerInterrupt(
        ((uint32_t) AESADV_INST_INT_IRQN), AESADV_INST_IRQHandler);

    NVIC_EnableIRQ(AESADV_INST_INT_IRQN);

    /* Load the 256-bit key */
    DL_AESADV_setKey(AESADV_INST, g_key, DL_AESADV_KEY_SIZE_256_BIT);

    /* Initialize CBC mode */
    SYSCFG_DL_AESADV_init();

    while (!inputReady) {
        __WFE();
    }

    /* Load plaintext data */
    DL_AESADV_loadInputData(AESADV_INST, g_plaintext);

    /* Wait for engine to complete operation */
    while (!outputReady) {
        __WFE();
    }

    /* Get ciphertext */
    DL_AESADV_readOutputData(AESADV_INST, ciphertext);

    /* Compare ciphertext to golden data set */
    gCorrectResults = true;
    for (i = 0; i < 16; i++) {
        gCorrectResults &= (g_goldenData[i] == ciphertext[i]);
    }

    /* gAESADV_configure for decryption */
    config->direction = DL_AESADV_DIR_DECRYPT;

    /* Initialize CBC mode for decryption */
    SYSCFG_DL_AESADV_init();

    /* Load ciphertext for decryption */
    DL_AESADV_loadInputData(AESADV_INST, ciphertext);

    DL_AESADV_enableInterrupt(AESADV_INST, DL_AESADV_INTERRUPT_OUTPUT_READY);

    outputReady = false;

    /* Wait for engine to complete operation */
    while (!outputReady) {
        __WFE();
    }

    /* Get decrypted result */
    DL_AESADV_readOutputData(AESADV_INST, buffer);

    /* Compare decrypted data to original plaintext */
    gCorrectResults = true;
    for (i = 0; i < 16; i++) {
        gCorrectResults &= (buffer[i] == g_plaintext[i]);
    }

#ifdef __SLATE_INTERNAL__
    gTestComplete = true;
#endif

    if (gCorrectResults) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    /* Stop for debug */
    __BKPT(0);

    while (1) {
        __WFE();
    }
}
