/*
 * MSPM33 AES Hardware Accelerator (AESADV) Driver Context Definitions
 *
 * Copyright (c) 2026, Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TI_MSPM33_AES_CTX_H
#define TI_MSPM33_AES_CTX_H

#include <stdint.h>
#include <stddef.h>
#include <psa/crypto.h>

/* TI driverlib types are intentionally excluded from this header. It is
 * included transitively from psa/crypto.h, so it must not depend on SDK
 * headers. The implementation (.c) includes dl_aesadv.h directly and casts
 * the opaque pointers/values back to their driverlib types. */

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Constants
 * ============================================================================ */

/** AES block size in bytes (hardware constraint) */
#define TI_MSPM33_AES_BLOCK_SIZE   (16U)

/** Maximum AES key size in bytes (256-bit) */
#define TI_MSPM33_AES_MAX_KEY_SIZE (32U)

/* ============================================================================
 * Cipher Operation Context
 * ============================================================================ */

/**
 * @brief Cipher operation state for AES-ECB/CBC/CTR multipart operations
 *
 * Embedded within psa_driver_cipher_context_t union when
 * CRYPTO_HW_USE_MSPM33_AES is enabled.
 */
typedef struct {
    /**
     * Opaque pointer to AESADV hardware register structure (AESADV_Regs *).
     * Stored as void * to avoid pulling TI SDK headers into this public
     * context header. Cast to AESADV_Regs * in the implementation file.
     */
    void *aesadv;

    /** PSA algorithm for this operation. */
    psa_algorithm_t alg;

    /**
     * Key material copy. PSA core only provides the key during setup(),
     * not across subsequent update/finish calls.
     */
    uint8_t key_material[TI_MSPM33_AES_MAX_KEY_SIZE];

    /** Key size in bytes (16 for AES-128, 32 for AES-256). */
    size_t key_size;

    /**
     * Current IV / counter state. For CBC this holds the chaining value
     * updated after each update() call. For CTR this holds the current
     * counter block updated after each update() call.
     */
    uint8_t iv[TI_MSPM33_AES_BLOCK_SIZE];

    /** 0 = encrypt, 1 = decrypt. */
    uint8_t direction;

    /**
     * 0 = IV not yet set (hardware not yet initialised for CBC/CTR).
     * 1 = IV set; hardware is ready to process data.
     * For ECB (no IV), this is set to 1 on the first update() call.
     */
    uint8_t iv_set;

    /** 0 = inactive (no active operation), 1 = active. */
    uint8_t active;

    /** Partial block accumulator — holds bytes not yet submitted to hardware. */
    uint8_t buffer[TI_MSPM33_AES_BLOCK_SIZE];

    /** Number of valid bytes currently in buffer. Range: [0, 15]. */
    size_t buffer_length;
} ti_mspm33_cipher_operation_t;

#ifdef __cplusplus
}
#endif

#endif /* TI_MSPM33_AES_CTX_H */
