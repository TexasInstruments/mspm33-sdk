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
 * AES-128-CBC example using TF-PSA-Crypto APIs with AESADV hardware acceleration.
 *
 * Test vectors from NIST SP 800-38A, Section F.2.1 (CBC-AES128 Encrypt).
 *
 * Pass: USER_LED_3 (Green, PC27) lights
 * Fail: USER_LED_2 (Red,   PC26) lights
 */

#include <psa/crypto.h>
#include <string.h>
#include <ti/driverlib/dl_trng.h>
#include "ti_msp_dl_config.h"

#define CIPHER_ALG PSA_ALG_CBC_NO_PADDING
#define KEY_BITS 128
#define IV_LENGTH 16
#define BLOCK_SIZE 16

/* AES-128 key — NIST SP 800-38A F.2.1 */
static const uint8_t aes_key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2,
    0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

/* IV */
static const uint8_t iv[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

/* Plaintext (64 bytes = 4 blocks) */
static const uint8_t plaintext[]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f,
    0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a,
    0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e,
    0x51, 0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1,
    0x19, 0x1a, 0x0a, 0x52, 0xef, 0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b,
    0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
static const size_t plaintext_len = sizeof(plaintext);

/* Expected ciphertext — NIST SP 800-38A F.2.1 */
static const uint8_t expected_ciphertext[] = {0x76, 0x49, 0xab, 0xac, 0x81,
    0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d, 0x50,
    0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91,
    0x76, 0x78, 0xb2, 0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71,
    0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16, 0x3f, 0xf1, 0xca, 0xa1, 0x68,
    0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7};

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

    /* psa_cipher_encrypt output = IV (16 bytes) || ciphertext (plaintext_len) */
    uint8_t ciphertext[PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(
        PSA_KEY_TYPE_AES, CIPHER_ALG, sizeof(plaintext))];
    size_t ciphertext_len;

    uint8_t decrypted[sizeof(plaintext)];
    size_t decrypted_len;

    /* Import AES-128 key */
    psa_set_key_usage_flags(
        &attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, CIPHER_ALG);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, KEY_BITS);

    status = psa_import_key(&attributes, aes_key, sizeof(aes_key), &key_id);
    if (status != PSA_SUCCESS) {
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    /* Encrypt with a known IV using the multi-part API so we can verify
     * against the NIST SP 800-38A F.2.1 test vectors. */
    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;

    status = psa_cipher_encrypt_setup(&enc_op, key_id, CIPHER_ALG);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    status = psa_cipher_set_iv(&enc_op, iv, sizeof(iv));
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&enc_op);
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    size_t enc_out1_len = 0u;
    status = psa_cipher_update(&enc_op, plaintext, plaintext_len, ciphertext,
        sizeof(ciphertext), &enc_out1_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&enc_op);
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    size_t enc_out2_len = 0u;
    status              = psa_cipher_finish(&enc_op, ciphertext + enc_out1_len,
        sizeof(ciphertext) - enc_out1_len, &enc_out2_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&enc_op);
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }
    ciphertext_len = enc_out1_len + enc_out2_len;

    /* Verify ciphertext against NIST SP 800-38A F.2.1 vectors */
    if (ciphertext_len != plaintext_len ||
        memcmp(ciphertext, expected_ciphertext, plaintext_len) != 0) {
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    /* Decrypt using one-shot API: build iv || ciphertext input buffer */
    uint8_t dec_input[IV_LENGTH + sizeof(ciphertext)];
    memcpy(dec_input, iv, IV_LENGTH);
    memcpy(dec_input + IV_LENGTH, ciphertext, ciphertext_len);

    status = psa_cipher_decrypt(key_id, CIPHER_ALG, dec_input,
        IV_LENGTH + ciphertext_len, decrypted, sizeof(decrypted),
        &decrypted_len);

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
