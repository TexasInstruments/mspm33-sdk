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
#include <ti/driverlib/dl_trng.h>
#include "ti_msp_dl_config.h"

#define KEY_BITS 128
#define TAG_LENGTH 8
#define NONCE_LENGTH 12
#define AEAD_ALG PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8)

/* AES-128 key */
const uint8_t aes_key[] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f};

/* Nonce (12 bytes for CCM) */
const uint8_t nonce[] = {
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b};
const size_t nonce_len = sizeof(nonce);

/* Additional authenticated data */
const uint8_t additional_data[]  = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
    0x13};
const size_t additional_data_len = sizeof(additional_data);

/* Plaintext */
const uint8_t plaintext[]  = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37};
const size_t plaintext_len = sizeof(plaintext);

int main(void)
{
    SYSCFG_DL_init();

    /* Initialize TRNG for entropy */
    DL_TRNG_reset(TRNG);
    DL_TRNG_enablePower(TRNG);
    DL_TRNG_setClockDivider(TRNG, DL_TRNG_CLOCK_DIVIDE_2);
    DL_TRNG_sendCommand(TRNG, DL_TRNG_CMD_NORM_FUNC);
    while (!DL_TRNG_isCommandDone(TRNG))
        ;

    volatile psa_status_t status;

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    psa_key_id_t key_id             = PSA_KEY_ID_NULL;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    uint8_t ciphertext[PSA_AEAD_ENCRYPT_OUTPUT_SIZE(
        PSA_KEY_TYPE_AES, AEAD_ALG, sizeof(plaintext))];
    size_t ciphertext_len;

    uint8_t decrypted[sizeof(plaintext)];
    size_t decrypted_len;

    /* Import AES key */
    psa_set_key_usage_flags(
        &attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, AEAD_ALG);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, KEY_BITS);

    status = psa_import_key(&attributes, aes_key, sizeof(aes_key), &key_id);
    if (status != PSA_SUCCESS) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    /* Encrypt */
    status = psa_aead_encrypt(key_id, AEAD_ALG, nonce, nonce_len,
        additional_data, additional_data_len, plaintext, plaintext_len,
        ciphertext, sizeof(ciphertext), &ciphertext_len);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    /* Decrypt and verify */
    status = psa_aead_decrypt(key_id, AEAD_ALG, nonce, sizeof(nonce),
        additional_data, additional_data_len, ciphertext, ciphertext_len,
        decrypted, sizeof(decrypted), &decrypted_len);

    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS || decrypted_len != plaintext_len ||
        memcmp(decrypted, plaintext, plaintext_len) != 0) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    }

    while (1) {
    }
}
