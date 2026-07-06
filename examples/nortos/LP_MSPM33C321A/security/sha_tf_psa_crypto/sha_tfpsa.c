/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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

#include <psa/crypto.h>
#include <string.h>
#include "ti_msp_dl_config.h"

#define HASH_ALG PSA_ALG_SHA_256

const uint8_t sample_message[]     = "Hello World!";
const size_t sample_message_length = sizeof(sample_message) - 1;

const uint8_t expected_hash[]  = {0x7f, 0x83, 0xb1, 0x65, 0x7f, 0xf1, 0xfc,
    0x53, 0xb9, 0x2d, 0xc1, 0x81, 0x48, 0xa1, 0xd6, 0x5d, 0xfc, 0x2d, 0x4b,
    0x1f, 0xa3, 0xd6, 0x77, 0x28, 0x4a, 0xdd, 0xd2, 0x00, 0x12, 0x6d, 0x90,
    0x69};
const size_t expected_hash_len = sizeof(expected_hash);

int main(void)
{
    volatile psa_status_t status;
    uint8_t hash[PSA_HASH_LENGTH(HASH_ALG)];
    size_t hash_length;

    SYSCFG_DL_init();

    status = psa_hash_compute(HASH_ALG, sample_message, sample_message_length,
        hash, sizeof(hash), &hash_length);

    if (status != PSA_SUCCESS || hash_length != expected_hash_len ||
        memcmp(hash, expected_hash, expected_hash_len) != 0) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    }

    while (1) {
    }
}
