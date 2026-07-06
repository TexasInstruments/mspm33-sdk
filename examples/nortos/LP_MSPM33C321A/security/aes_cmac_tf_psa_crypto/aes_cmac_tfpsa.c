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
 * AES-128-CMAC example using TF-PSA-Crypto APIs with AESADV hardware acceleration.
 *
 * Test vectors from NIST SP 800-38B, Examples 1-4.
 * Tests both one-shot (psa_mac_compute) and multi-part (setup/update/finish).
 *
 * Pass: USER_LED_3 (Green, PC27) lights
 * Fail: USER_LED_2 (Red,   PC26) lights
 */

#include <psa/crypto.h>
#include <stdio.h>
#include <string.h>
#include <ti/driverlib/dl_trng.h>
#include "ti_msp_dl_config.h"

#define MAC_ALG PSA_ALG_CMAC
#define KEY_BITS 128

/* NIST SP 800-38B Example 1: AES-128-CMAC with variable message length */

/* AES-128 key — shared across all test vectors */
static const uint8_t aes_key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2,
    0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};

/* Test vector 1: Empty message (0 bytes) */
static const uint8_t msg1[]          = {};
static const size_t msg1_len         = 0;
static const uint8_t expected_mac1[] = {0xbb, 0x1d, 0x69, 0x29, 0xe9, 0x59,
    0x37, 0x28, 0x7f, 0xa3, 0x7d, 0x12, 0x9b, 0x75, 0x67, 0x46};

/* Test vector 2: 16-byte message */
static const uint8_t msg2[]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
static const size_t msg2_len = 16;
static const uint8_t expected_mac2[] = {0x07, 0x0a, 0x16, 0xb4, 0x6b, 0x4d,
    0x41, 0x44, 0xf7, 0x9b, 0xdd, 0x9d, 0xd0, 0x4a, 0x28, 0x7c};

/* Test vector 3: 40-byte message */
static const uint8_t msg3[]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a, 0x57,
    0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11};
static const size_t msg3_len = 40;
static const uint8_t expected_mac3[] = {0xdf, 0xa6, 0x67, 0x47, 0xde, 0x9a,
    0xe6, 0x30, 0x30, 0xca, 0x32, 0x61, 0x14, 0x97, 0xc8, 0x27};

/* Test vector 4: 64-byte message */
static const uint8_t msg4[]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, 0xae, 0x2d, 0x8a, 0x57,
    0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19,
    0x1a, 0x0a, 0x52, 0xef, 0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
static const size_t msg4_len = 64;
static const uint8_t expected_mac4[] = {0x51, 0xf0, 0xbe, 0xbf, 0x7e, 0x3b,
    0x9d, 0x92, 0xfc, 0x49, 0x74, 0x17, 0x79, 0x36, 0x3c, 0xfe};

static int test_one_shot(const char *name, const uint8_t *message,
    size_t msg_len, const uint8_t *expected_mac)
{
    uint8_t computed_mac[16];
    size_t computed_mac_len;
    psa_status_t status;

    printf("  Testing %s (one-shot)...\n", name);

    /* One-shot: setup + update + finish */
    psa_mac_operation_t mac_op      = PSA_MAC_OPERATION_INIT;
    psa_key_id_t key_id             = PSA_KEY_ID_NULL;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, MAC_ALG);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, KEY_BITS);

    /* Import key */
    status = psa_import_key(&attributes, aes_key, sizeof(aes_key), &key_id);
    if (status != PSA_SUCCESS) {
        printf("    ERROR: psa_import_key failed (status=%d)\n", status);
        return 0;
    }

    /* Setup: start MAC operation */
    status = psa_mac_sign_setup(&mac_op, key_id, MAC_ALG);
    if (status != PSA_SUCCESS) {
        printf("    ERROR: psa_mac_sign_setup failed (status=%d)\n", status);
        psa_destroy_key(key_id);
        return 0;
    }

    /* Update: feed entire message at once */
    if (msg_len > 0) {
        status = psa_mac_update(&mac_op, message, msg_len);
        if (status != PSA_SUCCESS) {
            printf("    ERROR: psa_mac_update failed (status=%d)\n", status);
            psa_mac_abort(&mac_op);
            psa_destroy_key(key_id);
            return 0;
        }
    }

    /* Finish: retrieve MAC */
    status = psa_mac_sign_finish(
        &mac_op, computed_mac, sizeof(computed_mac), &computed_mac_len);
    if (status != PSA_SUCCESS) {
        printf("    ERROR: psa_mac_sign_finish failed (status=%d)\n", status);
        psa_destroy_key(key_id);
        return 0;
    }

    psa_destroy_key(key_id);

    if (computed_mac_len != 16) {
        printf("    ERROR: MAC length mismatch (got %zu, expected 16)\n",
            computed_mac_len);
        return 0;
    }

    if (memcmp(computed_mac, expected_mac, 16) != 0) {
        printf("    ERROR: MAC mismatch\n");
        printf("    Expected: ");
        for (int i = 0; i < 16; i++) printf("%02x ", expected_mac[i]);
        printf("\n    Got:      ");
        for (int i = 0; i < 16; i++) printf("%02x ", computed_mac[i]);
        printf("\n");
        return 0;
    }

    printf("    PASS\n");
    return 1;
}

static int test_multi_part(const char *name, const uint8_t *message,
    size_t msg_len, const uint8_t *expected_mac)
{
    uint8_t computed_mac[16];
    size_t computed_mac_len;
    psa_status_t status;

    printf("  Testing %s (multi-part)...\n", name);

    /* Test multi-part: setup + update + finish */
    psa_mac_operation_t mac_op      = PSA_MAC_OPERATION_INIT;
    psa_key_id_t key_id             = PSA_KEY_ID_NULL;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_set_key_usage_flags(&attributes,
        PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, MAC_ALG);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, KEY_BITS);

    /* Import key */
    status = psa_import_key(&attributes, aes_key, sizeof(aes_key), &key_id);
    if (status != PSA_SUCCESS) {
        printf("    ERROR: psa_import_key failed (status=%d)\n", status);
        return 0;
    }

    /* Setup: start MAC operation */
    status = psa_mac_sign_setup(&mac_op, key_id, MAC_ALG);
    if (status != PSA_SUCCESS) {
        printf("    ERROR: psa_mac_sign_setup failed (status=%d)\n", status);
        psa_destroy_key(key_id);
        return 0;
    }

    /* Update: feed message data (test with two chunks for variety) */
    size_t chunk_size = (msg_len > 0) ? (msg_len / 2 + 1) : 0;
    if (chunk_size > 0) {
        status = psa_mac_update(&mac_op, message, chunk_size);
        if (status != PSA_SUCCESS) {
            printf("    ERROR: psa_mac_update failed (status=%d)\n", status);
            psa_mac_abort(&mac_op);
            psa_destroy_key(key_id);
            return 0;
        }

        if (msg_len > chunk_size) {
            status = psa_mac_update(
                &mac_op, message + chunk_size, msg_len - chunk_size);
            if (status != PSA_SUCCESS) {
                printf(
                    "    ERROR: psa_mac_update failed (status=%d)\n", status);
                psa_mac_abort(&mac_op);
                psa_destroy_key(key_id);
                return 0;
            }
        }
    }

    /* Finish: retrieve MAC */
    status = psa_mac_sign_finish(
        &mac_op, computed_mac, sizeof(computed_mac), &computed_mac_len);
    if (status != PSA_SUCCESS) {
        printf("    ERROR: psa_mac_sign_finish failed (status=%d)\n", status);
        psa_destroy_key(key_id);
        return 0;
    }

    psa_destroy_key(key_id);

    if (computed_mac_len != 16) {
        printf("    ERROR: MAC length mismatch (got %zu, expected 16)\n",
            computed_mac_len);
        return 0;
    }

    if (memcmp(computed_mac, expected_mac, 16) != 0) {
        printf("    ERROR: MAC mismatch\n");
        printf("    Expected: ");
        for (int i = 0; i < 16; i++) printf("%02x ", expected_mac[i]);
        printf("\n    Got:      ");
        for (int i = 0; i < 16; i++) printf("%02x ", computed_mac[i]);
        printf("\n");
        return 0;
    }

    printf("    PASS\n");
    return 1;
}

int main(void)
{
    SYSCFG_DL_init();
    printf("AES-CMAC Test Starting...\n");

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

    int all_pass = 1;

    /* Test all 4 NIST vectors with both one-shot and multi-part */
    printf("\nNIST SP 800-38B Test Vector 1 (Empty message):\n");
    all_pass &= test_one_shot("Empty", msg1, msg1_len, expected_mac1);
    all_pass &= test_multi_part("Empty", msg1, msg1_len, expected_mac1);

    printf("\nNIST SP 800-38B Test Vector 2 (16 bytes = 1 block):\n");
    all_pass &= test_one_shot("1 Block", msg2, msg2_len, expected_mac2);
    all_pass &= test_multi_part("1 Block", msg2, msg2_len, expected_mac2);

    printf("\nNIST SP 800-38B Test Vector 3 (40 bytes = 2.5 blocks):\n");
    all_pass &= test_one_shot("2.5 Blocks", msg3, msg3_len, expected_mac3);
    all_pass &= test_multi_part("2.5 Blocks", msg3, msg3_len, expected_mac3);

    printf("\nNIST SP 800-38B Test Vector 4 (64 bytes = 4 blocks):\n");
    all_pass &= test_one_shot("4 Blocks", msg4, msg4_len, expected_mac4);
    all_pass &= test_multi_part("4 Blocks", msg4, msg4_len, expected_mac4);

    printf("\n");
    if (all_pass) {
        printf("✓ ALL TESTS PASSED\n");
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_3_PORT, GPIO_LEDS_USER_LED_3_PIN);
    } else {
        printf("✗ SOME TESTS FAILED\n");
        DL_GPIO_setPins(GPIO_LEDS_USER_LED_2_PORT, GPIO_LEDS_USER_LED_2_PIN);
    }

    while (1) {
    }
}
