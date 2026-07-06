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
 * AES-128-CFB and AES-128-OFB example using TF-PSA-Crypto APIs
 * with MSPM33 AESADV hardware acceleration.
 *
 * Test vectors from NIST SP 800-38A:
 *   CFB: Section F.3.13 (CFB128-AES128)
 *   OFB: Section F.4.1  (OFB-AES128)
 *
 * Tests one-shot (psa_cipher_encrypt/decrypt) and
 * multi-part (setup/set_iv/update/finish) for both modes.
 *
 * Pass: USER_LED_3 (Green, PC27) lights
 * Fail: USER_LED_2 (Red,   PC26) lights
 */

#include <psa/crypto.h>
#include <stdio.h>
#include <string.h>
#include <ti/driverlib/dl_trng.h>
#include "ti_msp_dl_config.h"

/* AES-128 key — NIST SP 800-38A Appendix B */
static const uint8_t aes_key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2,
    0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

/* IV — NIST SP 800-38A Appendix B */
static const uint8_t iv[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

/* Plaintext — 32 bytes (2 blocks), NIST SP 800-38A Appendix B */
static const uint8_t plaintext[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f,
    0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a,
    0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e,
    0x51};

/* Expected CFB ciphertext — NIST SP 800-38A F.3.13 */
static const uint8_t expected_cfb_ct[] = {0x3b, 0x3f, 0xd9, 0x2e, 0xb7, 0x2d,
    0xad, 0x20, 0x33, 0x34, 0x49, 0xf8, 0xe8, 0x3c, 0xfb, 0x4a, 0xc8, 0xa6,
    0x45, 0x37, 0xa0, 0xb3, 0xa9, 0x3f, 0xcd, 0xe3, 0xcd, 0xad, 0x9f, 0x1c,
    0xe5, 0x8b};

/* Expected OFB ciphertext — NIST SP 800-38A F.4.1 */
static const uint8_t expected_ofb_ct[] = {0x3b, 0x3f, 0xd9, 0x2e, 0xb7, 0x2d,
    0xad, 0x20, 0x33, 0x34, 0x49, 0xf8, 0xe8, 0x3c, 0xfb, 0x4a, 0x77, 0x89,
    0x50, 0x8d, 0x16, 0x91, 0x8f, 0x03, 0xf5, 0x3c, 0x52, 0xda, 0xc5, 0x4e,
    0xd8, 0x25};

/*
 * Test one mode (CFB or OFB) using both one-shot and multi-part APIs.
 * Returns 1 on all-pass, 0 on any failure.
 */
static int test_mode(
    const char *name, psa_algorithm_t alg, const uint8_t *expected_ct)
{
    psa_status_t status;
    psa_key_id_t key_id       = PSA_KEY_ID_NULL;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

    printf("\n%s:\n", name);

    psa_set_key_usage_flags(
        &attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attr, alg);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attr, 128);

    status = psa_import_key(&attr, aes_key, sizeof(aes_key), &key_id);
    if (status != PSA_SUCCESS) {
        printf("  ERROR: psa_import_key failed (%d)\n", status);
        return 0;
    }

    /* --- One-shot encrypt --- */
    /* psa_cipher_encrypt prepends IV; output = iv || ciphertext */
    uint8_t enc_out[sizeof(iv) + sizeof(plaintext)];
    size_t enc_out_len = 0;

    printf("  One-shot encrypt... ");
    status = psa_cipher_encrypt(key_id, alg, plaintext, sizeof(plaintext),
        enc_out, sizeof(enc_out), &enc_out_len);
    if (status != PSA_SUCCESS || enc_out_len != sizeof(enc_out)) {
        printf("FAIL (status=%d, len=%zu)\n", status, enc_out_len);
        psa_destroy_key(key_id);
        return 0;
    }
    /* enc_out[0..15] = random IV; enc_out[16..47] = ciphertext.
     * One-shot uses a random IV so we can't compare ciphertext here —
     * instead verify round-trip via one-shot decrypt below. */
    printf("OK\n");

    /* --- One-shot decrypt (round-trip check) --- */
    uint8_t dec_out[sizeof(plaintext)];
    size_t dec_out_len = 0;

    printf("  One-shot decrypt (round-trip)... ");
    status = psa_cipher_decrypt(key_id, alg, enc_out, enc_out_len, dec_out,
        sizeof(dec_out), &dec_out_len);
    if (status != PSA_SUCCESS || dec_out_len != sizeof(plaintext) ||
        memcmp(dec_out, plaintext, sizeof(plaintext)) != 0) {
        printf("FAIL (status=%d)\n", status);
        psa_destroy_key(key_id);
        return 0;
    }
    printf("OK\n");

    /* --- Multi-part encrypt with known IV (verify against NIST vector) --- */
    psa_cipher_operation_t op = PSA_CIPHER_OPERATION_INIT;
    uint8_t mp_ct[sizeof(plaintext)];
    size_t mp_ct_len = 0, tmp_len = 0;

    printf("  Multi-part encrypt (NIST vector)... ");
    status = psa_cipher_encrypt_setup(&op, key_id, alg);
    if (status != PSA_SUCCESS) goto mp_fail;

    status = psa_cipher_set_iv(&op, iv, sizeof(iv));
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&op);
        goto mp_fail;
    }

    status = psa_cipher_update(
        &op, plaintext, sizeof(plaintext), mp_ct, sizeof(mp_ct), &mp_ct_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&op);
        goto mp_fail;
    }

    status = psa_cipher_finish(
        &op, mp_ct + mp_ct_len, sizeof(mp_ct) - mp_ct_len, &tmp_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&op);
        goto mp_fail;
    }

    mp_ct_len += tmp_len;

    if (mp_ct_len != sizeof(plaintext) ||
        memcmp(mp_ct, expected_ct, sizeof(plaintext)) != 0) {
        printf("FAIL (vector mismatch)\n");
        printf("  Expected: ");
        for (size_t i = 0; i < sizeof(plaintext); i++)
            printf("%02x", expected_ct[i]);
        printf("\n  Got:      ");
        for (size_t i = 0; i < mp_ct_len; i++) printf("%02x", mp_ct[i]);
        printf("\n");
        psa_destroy_key(key_id);
        return 0;
    }
    printf("OK\n");

    /* --- Multi-part decrypt --- */
    op = (psa_cipher_operation_t) PSA_CIPHER_OPERATION_INIT;
    uint8_t mp_pt[sizeof(plaintext)];
    size_t mp_pt_len = 0;
    tmp_len          = 0;

    printf("  Multi-part decrypt... ");
    status = psa_cipher_decrypt_setup(&op, key_id, alg);
    if (status != PSA_SUCCESS) goto mp_fail;

    status = psa_cipher_set_iv(&op, iv, sizeof(iv));
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&op);
        goto mp_fail;
    }

    status = psa_cipher_update(
        &op, mp_ct, mp_ct_len, mp_pt, sizeof(mp_pt), &mp_pt_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&op);
        goto mp_fail;
    }

    status = psa_cipher_finish(
        &op, mp_pt + mp_pt_len, sizeof(mp_pt) - mp_pt_len, &tmp_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&op);
        goto mp_fail;
    }
    mp_pt_len += tmp_len;

    if (mp_pt_len != sizeof(plaintext) ||
        memcmp(mp_pt, plaintext, sizeof(plaintext)) != 0) {
        printf("FAIL (plaintext mismatch)\n");
        psa_destroy_key(key_id);
        return 0;
    }
    printf("OK\n");

    psa_destroy_key(key_id);
    return 1;

mp_fail:
    printf("FAIL (status=%d)\n", status);
    psa_destroy_key(key_id);
    return 0;
}

int main(void)
{
    SYSCFG_DL_init();
    printf("AES-CFB/OFB Test Starting...\n");

    DL_TRNG_reset(TRNG);
    DL_TRNG_enablePower(TRNG);
    DL_TRNG_setClockDivider(TRNG, DL_TRNG_CLOCK_DIVIDE_2);
    DL_TRNG_sendCommand(TRNG, DL_TRNG_CMD_NORM_FUNC);
    while (!DL_TRNG_isCommandDone(TRNG))
        ;

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("ERROR: psa_crypto_init failed (%d)\n", status);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }

    int all_pass = 1;
    all_pass &= test_mode(
        "AES-128-CFB (NIST SP 800-38A F.3.13)", PSA_ALG_CFB, expected_cfb_ct);
    all_pass &= test_mode(
        "AES-128-OFB (NIST SP 800-38A F.4.1)", PSA_ALG_OFB, expected_ofb_ct);

    if (all_pass) {
        printf("\nALL TESTS PASSED\n");
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    } else {
        printf("\nSOME TESTS FAILED\n");
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
    }
    while (1) {
    }
}
