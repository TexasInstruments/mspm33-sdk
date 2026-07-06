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

/*
 * AES-128-GCM example using TF-PSA-Crypto APIs with AESADV hardware acceleration.
 *
 * Test vectors from NIST SP 800-38D, Test Case 4 (AES-128, 12-byte IV, with AAD).
 *
 * Pass: USER_LED_3 (Green, PC27) lights
 * Fail: USER_LED_2 (Red,   PC26) lights
 */

#include <psa/crypto.h>
#include <string.h>
#include <ti/driverlib/dl_trng.h>
#include "ti_msp_dl_config.h"

#define AEAD_ALG PSA_ALG_GCM
#define KEY_BITS 128
#define TAG_LENGTH 16
#define NONCE_LENGTH 12

/* AES-128 key — NIST SP 800-38D Test Case 4 */
static const uint8_t aes_key[] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73,
    0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08};

/* Nonce (12 bytes) */
static const uint8_t nonce[] = {
    0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88};

/* Additional authenticated data (20 bytes) */
static const uint8_t additional_data[] = {0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad,
    0xbe, 0xef, 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xab, 0xad,
    0xda, 0xd2};

static const size_t additional_data_len = sizeof(additional_data);

/* Plaintext (60 bytes) */
static const uint8_t plaintext[] = {0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06,
    0xe5, 0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a, 0x86, 0xa7, 0xa9,
    0x53, 0x15, 0x34, 0xf7, 0xda, 0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a,
    0x72, 0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53, 0x2f, 0xcf, 0x0e,
    0x24, 0x49, 0xa6, 0xb5, 0x25, 0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6,
    0x57, 0xba, 0x63, 0x7b, 0x39};

static const size_t plaintext_len = sizeof(plaintext);

/* Expected ciphertext (60 bytes) — NIST SP 800-38D Test Case 4 */
static const uint8_t expected_ciphertext[] = {0x42, 0x83, 0x1e, 0xc2, 0x21,
    0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, 0xe3,
    0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29,
    0xac, 0xa1, 0x2e, 0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d,
    0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, 0x1b, 0xa3, 0x0b, 0x39, 0x6a,
    0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91};

/* Expected tag (16 bytes) */
static const uint8_t expected_tag[] = {0x5b, 0xc9, 0x4f, 0xbc, 0x32, 0x21,
    0xa5, 0xdb, 0x94, 0xfa, 0xe9, 0x5a, 0xe7, 0x12, 0x1a, 0x47};

int main(void)
{
    SYSCFG_DL_init();

    /* Initialize TRNG for PSA entropy */
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

    /* Import AES-128 key */
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
    status = psa_aead_encrypt(key_id, AEAD_ALG, nonce, sizeof(nonce),
        additional_data, additional_data_len, plaintext, plaintext_len,
        ciphertext, sizeof(ciphertext), &ciphertext_len);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    /* Verify ciphertext and tag against known NIST vectors */
    if (ciphertext_len != plaintext_len + TAG_LENGTH ||
        memcmp(ciphertext, expected_ciphertext, plaintext_len) != 0 ||
        memcmp(ciphertext + plaintext_len, expected_tag, TAG_LENGTH) != 0) {
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    /* Decrypt and verify round-trip */
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
