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
 * AES-128-CTR example using TF-PSA-Crypto APIs with AESADV hardware acceleration.
 *
 * Test vectors from NIST SP 800-38A, Section F.5.1 (CTR-AES128 Encrypt).
 *
 * Pass: USER_LED_3 (Green, PC27) lights
 * Fail: USER_LED_2 (Red,   PC26) lights
 */

#include <psa/crypto.h>
#include <stdio.h>
#include <string.h>
#include <ti/driverlib/dl_trng.h>
#include "ti_msp_dl_config.h"

#define CIPHER_ALG PSA_ALG_CTR
#define KEY_BITS 128
#define IV_LENGTH 16

/* AES-128 key */
static const uint8_t aes_key[] = {0xAE, 0x68, 0x52, 0xF8, 0x12, 0x10, 0x67,
    0xCC, 0x4B, 0xF7, 0xA5, 0x76, 0x55, 0x77, 0xF3, 0x9E};

/* Nonce/IV (initial counter value) */
static const uint8_t nonce[] = {0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

/* Plaintext (64 bytes = 4 blocks) */
static const uint8_t plaintext[]  = {0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20,
    0x62, 0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67};
static const size_t plaintext_len = sizeof(plaintext);

/* Expected ciphertext */
static const uint8_t expected_ciphertext[] = {0xE4, 0x09, 0x5D, 0x4F, 0xB7,
    0xA7, 0xB3, 0x79, 0x2D, 0x61, 0x75, 0xA3, 0x26, 0x13, 0x11, 0xB8};

int main(void)
{
    SYSCFG_DL_init();
    printf("AES-CTR Test Starting...\n");

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
        printf("ERROR: PSA crypto init failed (status=%d)\n", status);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }
    printf("PSA crypto initialized\n");

    psa_key_id_t key_id             = PSA_KEY_ID_NULL;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    /* psa_cipher_encrypt output = nonce (16 bytes) || ciphertext (plaintext_len) */
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

    /* Encrypt using multi-part API to verify against NIST SP 800-38A F.5.1 */
    psa_cipher_operation_t enc_op = PSA_CIPHER_OPERATION_INIT;

    status = psa_cipher_encrypt_setup(&enc_op, key_id, CIPHER_ALG);
    if (status != PSA_SUCCESS) {
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    status = psa_cipher_set_iv(&enc_op, nonce, sizeof(nonce));
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

    /* Verify ciphertext against NIST SP 800-38A F.5.1 vectors */
    printf("Ciphertext length: %d (expected %d)\n", ciphertext_len,
        plaintext_len);
    if (ciphertext_len != plaintext_len ||
        memcmp(ciphertext, expected_ciphertext, plaintext_len) != 0) {
        printf("ERROR: Encryption verification FAILED\n");
        psa_destroy_key(key_id);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }
    printf("Encryption verification PASSED\n");

    /* Decrypt using one-shot API: build nonce || ciphertext input buffer */
    uint8_t dec_input[IV_LENGTH + sizeof(ciphertext)];
    memcpy(dec_input, nonce, IV_LENGTH);
    memcpy(dec_input + IV_LENGTH, ciphertext, ciphertext_len);

    status = psa_cipher_decrypt(key_id, CIPHER_ALG, dec_input,
        IV_LENGTH + ciphertext_len, decrypted, sizeof(decrypted),
        &decrypted_len);

    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS || decrypted_len != plaintext_len ||
        memcmp(decrypted, plaintext, plaintext_len) != 0) {
        printf("ERROR: Decryption verification FAILED (status=%d, len=%d)\n",
            status, decrypted_len);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
    } else {
        printf("SUCCESS: All AES-CTR tests PASSED!\n");
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    }

    while (1) {
    }
}
