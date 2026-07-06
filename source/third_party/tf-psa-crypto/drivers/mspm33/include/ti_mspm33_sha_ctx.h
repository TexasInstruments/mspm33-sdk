/*
 * MSPM33 SHA Hardware Accelerator (SHAW) Driver Context Definitions
 *
 * Copyright (c) 2026, Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TI_MSPM33_SHA_CTX_H
#define TI_MSPM33_SHA_CTX_H

#include <stdint.h>
#include <stddef.h>
#include <psa/crypto.h>

/* TI driverlib types are intentionally excluded from this header. It is
 * pulled in transitively from psa/crypto.h, so it must not depend on SDK
 * headers. The implementation (.c) includes dl_shadrv.h directly and casts
 * the opaque pointers/values back to their driverlib types. */

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Constants
 * ============================================================================ */

/** Maximum supported key size for HMAC operations */
#define TI_MSPM33_MAX_HMAC_KEY_SIZE (256U)

/** SHA block size (hardware constraint) */
#define TI_MSPM33_SHA_BLOCK_SIZE (64U)

/** SHA-224 digest size in bytes */
#define TI_MSPM33_SHA224_DIGEST_SIZE (28U)

/** SHA-256 digest size in bytes */
#define TI_MSPM33_SHA256_DIGEST_SIZE (32U)

/* ============================================================================
 * Hash Operation Context
 * ============================================================================ */

/**
 * @brief Hash operation state for SHA-224/256 multipart operations
 *
 * Embedded within psa_driver_hash_context_t union when
 * CRYPTO_HW_USE_MSPM33_SHA is enabled.
 */
typedef struct {
    /**
     * Opaque pointer to SHAW hardware register structure (SHAW_Regs *).
     * Stored as void * to avoid pulling TI SDK headers into this public
     * context header. Cast to SHAW_Regs * in the implementation file.
     */
    void *shaw;

    /**
     * Selected SHA algorithm stored as raw uint32_t.
     * Holds DL_SHAW_ALGORITHM_SHA_256 or DL_SHAW_ALGORITHM_SHA_224 values.
     * Cast to DL_SHAW_ALGORITHM in the implementation where required.
     */
    uint32_t algorithm;

    /**
     * Partial block buffer for accumulating data between update() calls.
     * SHAW hardware requires 64-byte aligned block submission.
     */
    uint8_t buffer[TI_MSPM33_SHA_BLOCK_SIZE];

    /** Current number of valid bytes in buffer. Range: [0, 63]. */
    size_t buffer_length;

    /** Total bytes of input data processed across all update() calls. */
    uint64_t total_length;

    /** Expected digest output size: 28 (SHA-224) or 32 (SHA-256). */
    size_t digest_size;

    /** 0 = inactive, non-zero = active (setup called, awaiting update/finish). */
    uint8_t active;
} ti_mspm33_hash_operation_t;

/* ============================================================================
 * MAC (HMAC) Operation Context
 * ============================================================================ */

/**
 * @brief MAC (HMAC) operation state for HMAC-SHA multipart operations
 *
 * Embedded within psa_driver_mac_context_t union when
 * CRYPTO_HW_USE_MSPM33_SHA is enabled.
 */
typedef struct {
    /** Opaque pointer to SHAW hardware register structure (SHAW_Regs *). */
    void *shaw;

    /** Selected HMAC algorithm as raw uint32_t (DL_SHAW_ALGORITHM value). */
    uint32_t algorithm;

    /**
     * Extracted HMAC key material. Stored here because PSA core only
     * provides the key pointer during mac_setup(), not across subsequent
     * update/finish calls.
     */
    uint8_t key_material[TI_MSPM33_MAX_HMAC_KEY_SIZE];

    /** Size of key material in bytes. Must be <= TI_MSPM33_MAX_HMAC_KEY_SIZE. */
    size_t key_size;

    /** Partial block buffer. */
    uint8_t buffer[TI_MSPM33_SHA_BLOCK_SIZE];

    /** Current number of valid bytes in buffer. */
    size_t buffer_length;

    /** Total bytes of message data (excluding key) processed. */
    uint64_t total_length;

    /** Expected digest output size: 28 (HMAC-SHA-224) or 32 (HMAC-SHA-256). */
    size_t digest_size;

    /** 0 = inactive, 1 = inner hash phase, 2 = outer hash phase. */
    uint8_t active;
} ti_mspm33_mac_operation_t;

#ifdef __cplusplus
}
#endif

#endif /* TI_MSPM33_SHA_CTX_H */
