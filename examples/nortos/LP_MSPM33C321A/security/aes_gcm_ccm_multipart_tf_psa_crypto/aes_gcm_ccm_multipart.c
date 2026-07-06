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
 * Multi-part AEAD example using TF-PSA-Crypto APIs with MSPM33 AESADV HW.
 *
 * Tests:
 *   1. AES-128-GCM  — NIST SP 800-38D Test Case 4
 *   2. AES-256-GCM  — same vectors with 256-bit key
 *   3. AES-128-CCM  — RFC 3610 Test Case 1
 *
 * Each test exercises the full multi-part flow:
 *   psa_aead_encrypt_setup → set_nonce → set_lengths →
 *   update_ad → update × 2 → finish
 * followed by:
 *   psa_aead_decrypt_setup → ... → verify (correct tag = PASS,
 *                                            tampered tag = PSA_ERROR_INVALID_SIGNATURE)
 *
 * Pass: USER_LED_3 (Green, PC27) lights
 * Fail: USER_LED_2 (Red,   PC26) lights
 */

#include <psa/crypto.h>
#include <stdio.h>
#include <string.h>
#include <ti/driverlib/dl_trng.h>
#include "ti_msp_dl_config.h"

/* -------------------------------------------------------------------------
 * Test vectors
 * ---------------------------------------------------------------------- */

/* NIST SP 800-38D Test Case 4 — AES-128-GCM */
static const uint8_t gcm128_key[] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73,
    0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08};
static const uint8_t gcm_iv[]     = {
    0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88};
static const uint8_t gcm_aad[] = {0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe,
    0xef, 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xda,
    0xd2};
static const uint8_t gcm_pt[]  = {0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06,
    0xe5, 0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a, 0x86, 0xa7, 0xa9,
    0x53, 0x15, 0x34, 0xf7, 0xda, 0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a,
    0x72, 0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53, 0x2f, 0xcf, 0x0e,
    0x24, 0x49, 0xa6, 0xb5, 0x25, 0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6,
    0x57, 0xba, 0x63, 0x7b, 0x39};
static const uint8_t gcm128_expected_ct[]  = {0x42, 0x83, 0x1e, 0xc2, 0x21,
    0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, 0xe3,
    0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29,
    0xac, 0xa1, 0x2e, 0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d,
    0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, 0x1b, 0xa3, 0x0b, 0x39, 0x6a,
    0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91};
static const uint8_t gcm128_expected_tag[] = {0x5b, 0xc9, 0x4f, 0xbc, 0x32,
    0x21, 0xa5, 0xdb, 0x94, 0xfa, 0xe9, 0x5a, 0xe7, 0x12, 0x1a, 0x47};

/* AES-256-GCM — same IV/AAD/PT as above, 256-bit key */
static const uint8_t gcm256_key[] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73,
    0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08, 0xfe, 0xff, 0xe9,
    0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83,
    0x08};
static const uint8_t gcm256_expected_ct[]  = {0x52, 0x2d, 0xc1, 0xf0, 0x99,
    0x56, 0x7d, 0x07, 0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d, 0x64,
    0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9, 0x75, 0x98, 0xa2, 0xbd, 0x25,
    0x55, 0xd1, 0xaa, 0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d, 0xa7,
    0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38, 0xc5, 0xf6, 0x1e, 0x63, 0x93,
    0xba, 0x7a, 0x0a, 0xbc, 0xc9, 0xf6, 0x62};
static const uint8_t gcm256_expected_tag[] = {0x76, 0xfc, 0x6e, 0xce, 0x0f,
    0x4e, 0x17, 0x68, 0xcd, 0xdf, 0x88, 0x53, 0xbb, 0x2d, 0x55, 0x1b};

/* RFC 3610 Test Case 1 — AES-128-CCM, 8-byte tag, 7-byte nonce */
static const uint8_t ccm_key[]   = {0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6,
    0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf};
static const uint8_t ccm_nonce[] = {0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5};
static const uint8_t ccm_aad[]   = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static const uint8_t ccm_pt[] = {0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
    0x1b, 0x1c, 0x1d, 0x1e};
static const uint8_t ccm_expected_ct[]  = {0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6,
    0x63, 0xd2, 0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80, 0x6d, 0x5f,
    0x6b, 0x61, 0xda, 0xc3, 0x84};
static const uint8_t ccm_expected_tag[] = {
    0x17, 0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0};

/* -------------------------------------------------------------------------
 * Multi-part AEAD test runner
 * ---------------------------------------------------------------------- */
static int test_multipart(const char *name, psa_algorithm_t alg,
    const uint8_t *key, size_t key_len, const uint8_t *nonce, size_t nonce_len,
    const uint8_t *aad, size_t aad_len, const uint8_t *pt, size_t pt_len,
    const uint8_t *exp_ct, const uint8_t *exp_tag, size_t tag_len)
{
    psa_status_t status;
    psa_key_id_t key_id       = PSA_KEY_ID_NULL;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;

    printf("\n%s:\n", name);

    psa_set_key_usage_flags(
        &attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attr, alg);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attr, (psa_key_bits_t)(key_len * 8u));

    status = psa_import_key(&attr, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        printf("  ERROR: psa_import_key (status=%d)\n", status);
        return 0;
    }

    /* --- Multi-part encrypt --- */
    printf("  Multi-part encrypt... ");
    psa_aead_operation_t enc_op = PSA_AEAD_OPERATION_INIT;
    uint8_t ct_buf[128]         = {0};
    uint8_t tag_buf[16]         = {0};
    size_t ct_len = 0, tag_out_len = 0, tmp = 0;

    status = psa_aead_encrypt_setup(&enc_op, key_id, alg);
    if (status != PSA_SUCCESS) {
        printf("FAIL setup (%d)\n", status);
        goto fail;
    }

    status = psa_aead_set_nonce(&enc_op, nonce, nonce_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL set_nonce (%d)\n", status);
        psa_aead_abort(&enc_op);
        goto fail;
    }

    status = psa_aead_set_lengths(&enc_op, aad_len, pt_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL set_lengths (%d)\n", status);
        psa_aead_abort(&enc_op);
        goto fail;
    }

    if (aad_len > 0) {
        /* Feed AAD in two halves to exercise multi-call update_ad */
        size_t half = aad_len / 2u;
        status      = psa_aead_update_ad(&enc_op, aad, half);
        if (status != PSA_SUCCESS) {
            printf("FAIL update_ad1 (%d)\n", status);
            psa_aead_abort(&enc_op);
            goto fail;
        }
        status = psa_aead_update_ad(&enc_op, aad + half, aad_len - half);
        if (status != PSA_SUCCESS) {
            printf("FAIL update_ad2 (%d)\n", status);
            psa_aead_abort(&enc_op);
            goto fail;
        }
    }

    /* Feed plaintext in two halves */
    size_t half_pt = pt_len / 2u;
    status =
        psa_aead_update(&enc_op, pt, half_pt, ct_buf, sizeof(ct_buf), &ct_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL update1 (%d)\n", status);
        psa_aead_abort(&enc_op);
        goto fail;
    }

    status = psa_aead_update(&enc_op, pt + half_pt, pt_len - half_pt,
        ct_buf + ct_len, sizeof(ct_buf) - ct_len, &tmp);
    if (status != PSA_SUCCESS) {
        printf("FAIL update2 (%d)\n", status);
        psa_aead_abort(&enc_op);
        goto fail;
    }
    ct_len += tmp;

    status = psa_aead_finish(&enc_op, ct_buf + ct_len, sizeof(ct_buf) - ct_len,
        &tmp, tag_buf, tag_len, &tag_out_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL finish (%d)\n", status);
        goto fail;
    }
    ct_len += tmp;

    if (ct_len != pt_len) {
        printf("FAIL (ct_len=%zu != pt_len=%zu)\n", ct_len, pt_len);
        goto fail;
    }
    if (memcmp(ct_buf, exp_ct, pt_len) != 0) {
        printf("FAIL (ciphertext mismatch)\n");
        printf("  Expected: ");
        for (size_t i = 0; i < pt_len; i++) printf("%02x", exp_ct[i]);
        printf("\n");
        printf("  Got:      ");
        for (size_t i = 0; i < ct_len; i++) printf("%02x", ct_buf[i]);
        printf("\n");
        goto fail;
    }
    if (tag_out_len != tag_len || memcmp(tag_buf, exp_tag, tag_len) != 0) {
        printf("FAIL (tag mismatch)\n");
        printf("  Expected: ");
        for (size_t i = 0; i < tag_len; i++) printf("%02x", exp_tag[i]);
        printf("\n");
        printf("  Got:      ");
        for (size_t i = 0; i < tag_out_len; i++) printf("%02x", tag_buf[i]);
        printf("\n");
        goto fail;
    }
    printf("OK\n");

    /* --- Multi-part verify (correct tag) --- */
    printf("  Multi-part verify (correct tag)... ");
    psa_aead_operation_t dec_op = PSA_AEAD_OPERATION_INIT;
    uint8_t pt_out[128]         = {0};
    size_t pt_out_len           = 0;
    tmp                         = 0;

    status = psa_aead_decrypt_setup(&dec_op, key_id, alg);
    if (status != PSA_SUCCESS) {
        printf("FAIL setup (%d)\n", status);
        goto fail;
    }
    status = psa_aead_set_nonce(&dec_op, nonce, nonce_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL set_nonce (%d)\n", status);
        psa_aead_abort(&dec_op);
        goto fail;
    }
    status = psa_aead_set_lengths(&dec_op, aad_len, pt_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL set_lengths (%d)\n", status);
        psa_aead_abort(&dec_op);
        goto fail;
    }
    if (aad_len > 0) {
        status = psa_aead_update_ad(&dec_op, aad, aad_len);
        if (status != PSA_SUCCESS) {
            printf("FAIL update_ad (%d)\n", status);
            psa_aead_abort(&dec_op);
            goto fail;
        }
    }
    status = psa_aead_update(
        &dec_op, ct_buf, ct_len, pt_out, sizeof(pt_out), &pt_out_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL update (%d)\n", status);
        psa_aead_abort(&dec_op);
        goto fail;
    }
    status = psa_aead_verify(&dec_op, pt_out + pt_out_len,
        sizeof(pt_out) - pt_out_len, &tmp, tag_buf, tag_len);
    if (status != PSA_SUCCESS) {
        printf("FAIL verify (%d)\n", status);
        goto fail;
    }
    pt_out_len += tmp;
    if (pt_out_len != pt_len || memcmp(pt_out, pt, pt_len) != 0) {
        printf("FAIL (plaintext mismatch)\n");
        goto fail;
    }
    printf("OK\n");

    /* --- Tampered tag should fail --- */
    printf("  Multi-part verify (tampered tag)... ");
    dec_op = (psa_aead_operation_t) PSA_AEAD_OPERATION_INIT;
    memset(pt_out, 0, sizeof(pt_out));
    pt_out_len = 0;
    tmp        = 0;

    uint8_t bad_tag[16];
    memcpy(bad_tag, tag_buf, tag_len);
    bad_tag[0] ^= 0xFF; /* flip first byte */

    status = psa_aead_decrypt_setup(&dec_op, key_id, alg);
    if (status != PSA_SUCCESS) {
        printf("FAIL setup (%d)\n", status);
        goto fail;
    }
    status = psa_aead_set_nonce(&dec_op, nonce, nonce_len);
    if (status != PSA_SUCCESS) {
        psa_aead_abort(&dec_op);
        goto fail;
    }
    status = psa_aead_set_lengths(&dec_op, aad_len, pt_len);
    if (status != PSA_SUCCESS) {
        psa_aead_abort(&dec_op);
        goto fail;
    }
    if (aad_len > 0) {
        status = psa_aead_update_ad(&dec_op, aad, aad_len);
        if (status != PSA_SUCCESS) {
            psa_aead_abort(&dec_op);
            goto fail;
        }
    }
    status = psa_aead_update(
        &dec_op, ct_buf, ct_len, pt_out, sizeof(pt_out), &pt_out_len);
    if (status != PSA_SUCCESS) {
        psa_aead_abort(&dec_op);
        goto fail;
    }
    status = psa_aead_verify(&dec_op, pt_out + pt_out_len,
        sizeof(pt_out) - pt_out_len, &tmp, bad_tag, tag_len);
    if (status != PSA_ERROR_INVALID_SIGNATURE) {
        printf("FAIL (expected INVALID_SIGNATURE, got %d)\n", status);
        goto fail;
    }
    printf("OK (rejected as expected)\n");

    psa_destroy_key(key_id);
    return 1;

fail:
    psa_destroy_key(key_id);
    return 0;
}

/* -------------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------- */
int main(void)
{
    SYSCFG_DL_init();
    printf("AES GCM/CCM Multi-part AEAD Test Starting...\n");

    DL_TRNG_reset(TRNG);
    DL_TRNG_enablePower(TRNG);
    DL_TRNG_setClockDivider(TRNG, DL_TRNG_CLOCK_DIVIDE_2);
    DL_TRNG_sendCommand(TRNG, DL_TRNG_CMD_NORM_FUNC);
    while (!DL_TRNG_isCommandDone(TRNG)) {
        ;
    }

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("ERROR: psa_crypto_init failed (%d)\n", status);
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
        while (1) {
        }
    }
    printf("PSA crypto initialized\n");

    int all_pass = 1;

    all_pass &= test_multipart("AES-128-GCM (NIST SP 800-38D TC4)",
        PSA_ALG_GCM, gcm128_key, sizeof(gcm128_key), gcm_iv, sizeof(gcm_iv),
        gcm_aad, sizeof(gcm_aad), gcm_pt, sizeof(gcm_pt), gcm128_expected_ct,
        gcm128_expected_tag, 16u);

    all_pass &= test_multipart("AES-256-GCM (NIST SP 800-38D TC4 variant)",
        PSA_ALG_GCM, gcm256_key, sizeof(gcm256_key), gcm_iv, sizeof(gcm_iv),
        gcm_aad, sizeof(gcm_aad), gcm_pt, sizeof(gcm_pt), gcm256_expected_ct,
        gcm256_expected_tag, 16u);

    all_pass &= test_multipart("AES-128-CCM (RFC 3610 TC1, 8-byte tag)",
        PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8), ccm_key,
        sizeof(ccm_key), ccm_nonce, sizeof(ccm_nonce), ccm_aad,
        sizeof(ccm_aad), ccm_pt, sizeof(ccm_pt), ccm_expected_ct,
        ccm_expected_tag, 8u);

    printf("\n");
    if (all_pass) {
        printf("ALL TESTS PASSED\n");
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    } else {
        printf("SOME TESTS FAILED\n");
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
    }
    while (1) {
    }
}
