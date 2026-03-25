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
 * This module provides a thin abstraction over some of the crypto
 * primitives to make it easier to swap out the used crypto library.
 *
 * At this point, there are two choices: MCUBOOT_USE_MBED_TLS, or
 * MCUBOOT_USE_TINYCRYPT.  It is a compile error there is not exactly
 * one of these defined.
 */

#ifndef __BOOTUTIL_CRYPTO_AES_KW_H_
#define __BOOTUTIL_CRYPTO_AES_KW_H_

#include "mcuboot_config/mcuboot_config.h"

#if (defined(MCUBOOT_USE_MBED_TLS) + \
     defined(MCUBOOT_USE_TINYCRYPT)) != 1
    #error "One crypto backend must be defined: either MBED_TLS or TINYCRYPT"
#endif

#if defined(MCUBOOT_USE_MBED_TLS)
    #include <mbedtls/aes.h>
    #include <mbedtls/nist_kw.h>
#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
    #include <tinycrypt/aes.h>
    #include <tinycrypt/constants.h>
#endif /* MCUBOOT_USE_TINYCRYPT */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MCUBOOT_USE_MBED_TLS)
typedef mbedtls_nist_kw_context bootutil_aes_kw_context;
static inline void bootutil_aes_kw_init(bootutil_aes_kw_context *ctx)
{
    (void)mbedtls_nist_kw_init(ctx);
}

static inline void bootutil_aes_kw_drop(bootutil_aes_kw_context *ctx)
{
    /* XXX: config defines MBEDTLS_PLATFORM_NO_STD_FUNCTIONS so no need to free */
    /* (void)mbedtls_aes_free(ctx); */
    (void)ctx;
}

static inline int bootutil_aes_kw_set_unwrap_key(bootutil_aes_kw_context *ctx, const uint8_t *k, uint32_t klen)
{
    return mbedtls_nist_kw_setkey(ctx, MBEDTLS_CIPHER_ID_AES, k, klen * 8, 0);
}

static inline int bootutil_aes_kw_unwrap(bootutil_aes_kw_context *ctx, const uint8_t *wrapped_key, uint32_t wrapped_key_len, uint8_t *key, uint32_t key_len)
{
    size_t olen;
    return mbedtls_nist_kw_unwrap(ctx, MBEDTLS_KW_MODE_KW, wrapped_key, wrapped_key_len, key, &olen, key_len);
}
#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
typedef struct tc_aes_key_sched_struct bootutil_aes_kw_context;
static inline void bootutil_aes_kw_init(bootutil_aes_kw_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_aes_kw_drop(bootutil_aes_kw_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_aes_kw_set_unwrap_key(bootutil_aes_kw_context *ctx, const uint8_t *k, uint32_t klen)
{
    int rc;

    if (klen != 16) {
        return -1;
    }

    rc = tc_aes128_set_decrypt_key(ctx, k);
    if (rc != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    return 0;
}

/*
 * Implements AES key unwrapping following RFC-3394 section 2.2.2, using
 * tinycrypt for AES-128 decryption.
 */
static int bootutil_aes_kw_unwrap(bootutil_aes_kw_context *ctx, const uint8_t *wrapped_key, uint32_t wrapped_key_len, uint8_t *key, uint32_t key_len)
{
    uint8_t A[8];
    uint8_t B[16];
    int8_t i, j, k;

    if (wrapped_key_len != 24 || key_len != 16) {
        return -1;
    }

    for (k = 0; k < 8; k++) {
        A[k]       = wrapped_key[k];
        key[k]     = wrapped_key[8 + k];
        key[8 + k] = wrapped_key[16 + k];
    }

    for (j = 5; j >= 0; j--) {
        for (i = 2; i > 0; i--) {
            for (k = 0; k < 8; k++) {
                B[k] = A[k];
                B[8 + k] = key[((i-1) * 8) + k];
            }
            B[7] ^= 2 * j + i;
            if (tc_aes_decrypt((uint8_t *)&B, (uint8_t *)&B, ctx) != TC_CRYPTO_SUCCESS) {
                return -1;
            }
            for (k = 0; k < 8; k++) {
                A[k] = B[k];
                key[((i-1) * 8) + k] = B[8 + k];
            }
        }
    }

    for (i = 0, k = 0; i < 8; i++) {
        k |= A[i] ^ 0xa6;
    }
    if (k) {
        return -1;
    }

    return 0;
}
#endif /* MCUBOOT_USE_TINYCRYPT */

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_CRYPTO_AES_KW_H_ */
