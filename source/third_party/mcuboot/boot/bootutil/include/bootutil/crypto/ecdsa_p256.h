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

#ifndef __BOOTUTIL_CRYPTO_ECDSA_P256_H_
#define __BOOTUTIL_CRYPTO_ECDSA_P256_H_

#include "mcuboot_config/mcuboot_config.h"

#if (defined(MCUBOOT_USE_TINYCRYPT) + \
     defined(MCUBOOT_USE_CC310) + \
     defined(MCUBOOT_USE_TI_CRYPTO) + \
     defined(MCUBOOT_USE_MSP_CRYPTO) + \
     defined(MCUBOOT_USE_MBED_TLS)) != 1
    #error "One crypto backend must be defined: either CC310 or TINYCRYPT"
#endif

#if defined(MCUBOOT_USE_TINYCRYPT)
    #include <tinycrypt/ecc_dsa.h>
    #include <tinycrypt/constants.h>
    #define BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE (4 * 8)
#endif /* MCUBOOT_USE_TINYCRYPT */

#if defined(MCUBOOT_USE_CC310)
    #include <cc310_glue.h>
    #define BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE (4 * 8)
#endif /* MCUBOOT_USE_CC310 */

#if defined(MCUBOOT_USE_TI_CRYPTO)
    #define BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE (4 * 8)
#endif /* MCUBOOT_USE_TI_CRYPTO */

#if defined(MCUBOOT_USE_MSP_CRYPTO)
    #define BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE (4 * 8)
#endif /* MCUBOOT_USE_TI_CRYPTO */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MCUBOOT_USE_TINYCRYPT)
typedef uintptr_t bootutil_ecdsa_p256_context;
static inline void bootutil_ecdsa_p256_init(bootutil_ecdsa_p256_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_ecdsa_p256_drop(bootutil_ecdsa_p256_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_ecdsa_p256_verify(bootutil_ecdsa_p256_context *ctx, const uint8_t *pk, const uint8_t *hash, const uint8_t *sig)
{
    int rc;
    (void)ctx;
    rc = uECC_verify(pk, hash, BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE, sig, uECC_secp256r1());
    if (rc != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    return 0;
}
#endif /* MCUBOOT_USE_TINYCRYPT */

#if defined(MCUBOOT_USE_CC310)
typedef uintptr_t bootutil_ecdsa_p256_context;
static inline void bootutil_ecdsa_p256_init(bootutil_ecdsa_p256_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_ecdsa_p256_drop(bootutil_ecdsa_p256_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_ecdsa_p256_verify(bootutil_ecdsa_p256_context *ctx, uint8_t *pk, uint8_t *hash, uint8_t *sig)
{
    (void)ctx;
    return cc310_ecdsa_verify_secp256r1(hash, pk, sig, BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE);
}
#endif /* MCUBOOT_USE_CC310 */

#if defined(MCUBOOT_USE_TI_CRYPTO)
#include "ti-crypto/sl_crypto.h"
typedef void* bootutil_ecdsa_p256_context;
static inline void bootutil_ecdsa_p256_init(bootutil_ecdsa_p256_context *ctx)
{
    SlCrypto_ecdsa_p256_init();
}

static inline void bootutil_ecdsa_p256_drop(bootutil_ecdsa_p256_context *ctx)
{
    SlCrypto_ecdsa_p256_drop();
}

static inline int bootutil_ecdsa_p256_verify(bootutil_ecdsa_p256_context *ctx, const uint8_t *pk, const uint8_t *hash, const uint8_t *sig)
{
    return SlCrypto_ecdsa_p256_verify(pk, hash, sig);
}
#endif /* MCUBOOT_USE_TI_CRYPTO */

#if defined(MCUBOOT_USE_MSP_CRYPTO)
#include "msp-crypto/msp_crypto.h"
typedef void* bootutil_ecdsa_p256_context;
static inline void bootutil_ecdsa_p256_init(bootutil_ecdsa_p256_context *ctx)
{
    MSPCrypto_ecdsa_p256_init();
}

static inline void bootutil_ecdsa_p256_drop(bootutil_ecdsa_p256_context *ctx)
{
    MSPCrypto_ecdsa_p256_drop();
}

static inline int bootutil_ecdsa_p256_verify(bootutil_ecdsa_p256_context *ctx, const uint8_t *pk, const uint8_t *hash, const uint8_t *sig)
{
    return MSPCrypto_ecdsa_p256_verify(pk, hash, sig);
}
#endif /* MCUBOOT_USE_MSP_CRYPTO */

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_CRYPTO_ECDSA_P256_H_ */
