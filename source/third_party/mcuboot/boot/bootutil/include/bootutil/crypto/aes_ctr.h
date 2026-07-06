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

#ifndef __BOOTUTIL_CRYPTO_AES_CTR_H_
#define __BOOTUTIL_CRYPTO_AES_CTR_H_

#include <string.h>

#include "mcuboot_config/mcuboot_config.h"

#if (defined(MCUBOOT_USE_MBED_TLS) + \
     defined(MCUBOOT_USE_TINYCRYPT)) != 1
    #error "One crypto backend must be defined: either MBED_TLS or TINYCRYPT"
#endif

#if defined(MCUBOOT_USE_MBED_TLS)
    #include <mbedtls/aes.h>
    #define BOOTUTIL_CRYPTO_AES_CTR_KEY_SIZE (16)
    #define BOOTUTIL_CRYPTO_AES_CTR_BLOCK_SIZE (16)
#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
    #include <string.h>
    #include <tinycrypt/aes.h>
    #include <tinycrypt/ctr_mode.h>
    #include <tinycrypt/constants.h>
    #define BOOTUTIL_CRYPTO_AES_CTR_KEY_SIZE TC_AES_KEY_SIZE
    #define BOOTUTIL_CRYPTO_AES_CTR_BLOCK_SIZE TC_AES_BLOCK_SIZE
#endif /* MCUBOOT_USE_TINYCRYPT */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MCUBOOT_USE_MBED_TLS)
typedef mbedtls_aes_context bootutil_aes_ctr_context;
static inline void bootutil_aes_ctr_init(bootutil_aes_ctr_context *ctx)
{
    (void)mbedtls_aes_init(ctx);
}

static inline void bootutil_aes_ctr_drop(bootutil_aes_ctr_context *ctx)
{
    /* XXX: config defines MBEDTLS_PLATFORM_NO_STD_FUNCTIONS so no need to free */
    /* (void)mbedtls_aes_free(ctx); */
    (void)ctx;
}

static inline int bootutil_aes_ctr_set_key(bootutil_aes_ctr_context *ctx, const uint8_t *k)
{
    return mbedtls_aes_setkey_enc(ctx, k, BOOTUTIL_CRYPTO_AES_CTR_KEY_SIZE * 8);
}

static inline int bootutil_aes_ctr_encrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *m, uint32_t mlen, size_t blk_off, uint8_t *c)
{
    uint8_t stream_block[BOOTUTIL_CRYPTO_AES_CTR_BLOCK_SIZE];
    return mbedtls_aes_crypt_ctr(ctx, mlen, &blk_off, counter, stream_block, m, c);
}

static inline int bootutil_aes_ctr_decrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *c, uint32_t clen, size_t blk_off, uint8_t *m)
{
    uint8_t stream_block[BOOTUTIL_CRYPTO_AES_CTR_BLOCK_SIZE];
    return mbedtls_aes_crypt_ctr(ctx, clen, &blk_off, counter, stream_block, c, m);
}
#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
typedef struct tc_aes_key_sched_struct bootutil_aes_ctr_context;
static inline void bootutil_aes_ctr_init(bootutil_aes_ctr_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_aes_ctr_drop(bootutil_aes_ctr_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_aes_ctr_set_key(bootutil_aes_ctr_context *ctx, const uint8_t *k)
{
    int rc;
    rc = tc_aes128_set_encrypt_key(ctx, k);
    if (rc != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    return 0;
}

static int _bootutil_aes_ctr_crypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *in, uint32_t inlen, uint32_t blk_off, uint8_t *out)
{
    int rc;
    rc = tc_ctr_mode(out, inlen, in, inlen, counter, &blk_off, ctx);
    if (rc != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    return 0;
}

static inline int bootutil_aes_ctr_encrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *m, uint32_t mlen, uint32_t blk_off, uint8_t *c)
{
    return _bootutil_aes_ctr_crypt(ctx, counter, m, mlen, blk_off, c);
}

static inline int bootutil_aes_ctr_decrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *c, uint32_t clen, uint32_t blk_off, uint8_t *m)
{
    return _bootutil_aes_ctr_crypt(ctx, counter, c, clen, blk_off, m);
}
#endif /* MCUBOOT_USE_TINYCRYPT */

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_CRYPTO_AES_CTR_H_ */
