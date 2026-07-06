/*
 * Copyright (C) 2025 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 * Limited License.  
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell
 * ("Utilize") this software subject to the terms herein.  With respect to the
 * foregoing patent license, such license is granted  solely to the extent that
 * any such patent is necessary to Utilize the software alone.  The patent
 * license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI
 * (TI Devices).  No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this
 * license (including the above copyright notice and the disclaimer and (if
 * applicable) source code license limitations below) in the documentation
 * and/or other materials provided with the distribution.
 *
 * Redistribution and use in binary form, without modification, are permitted
 * provided that the following conditions are met:
 *
 *	* No reverse engineering, decompilation, or disassembly of this software is
 *     permitted with respect to any software provided in binary form.
 *	* Any redistribution and use are licensed by TI for use only with
 *     TI Devices.
 *	* Nothing shall obligate TI to provide you with source code for the software
 *     licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution
 * of the source code are permitted provided that the following conditions are
 * met:
 *
 *   * any redistribution and use of the source code, including any resulting
 *     derivative works, are licensed by TI for use only with TI Devices.
 *   * any redistribution and use of any object code compiled from the source
 *     code and any resulting derivative works, are licensed by TI for use only
 *     with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its
 * suppliers may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       sha.h
 *  @brief      SHA secure hash algorithm implementation
 * 
 *  This hash module implements the SHA-224, SHA-256, SHA-384, and SHA-512
 *  algorithms according to FIPS 180-4, as well as SHA3-224, SHA3-256,
 *  SHA3-384, and SHA3-512 variants from FIPS 202.
 *
 *  XOF functions SHAKE128 and SHAKE256 are implemented from FIPS 202.
 *
 *  SHA-224/256 integration notes:
 *  1. A maximum of 512MiB may be hashed by this implementation. (SHA2
 *     standard supports 2 million Terabytes for 224/256)
 *  2. Input data may be 8-bit aligned. However, output digest must be
 *     32-bit aligned.
 *  3. Intermediate values, input data, and final digest value may be
 *     left behind on the stack.
 *
 ******************************************************************************
 */

#ifndef CRYPTO_SHA_H_
#define CRYPTO_SHA_H_

#include <stddef.h>
#include <stdint.h>

/** @brief Digest length for SHA-224, in bytes */
#define CRYPTO_SHA224_DIGEST_LEN (28U)

/** @brief Digest length for SHA-256, in bytes */
#define CRYPTO_SHA256_DIGEST_LEN (32U)

/** @brief Digest length for SHA-384, in bytes */
#define CRYPTO_SHA384_DIGEST_LEN (48U)

/** @brief Digest length for SHA-512, in bytes */
#define CRYPTO_SHA512_DIGEST_LEN (64U)

/** @brief Digest length for SHA3-224, in bytes */
#define CRYPTO_SHA3_224_DIGEST_LEN (28U)

/** @brief Digest length for SHA3-256, in bytes */
#define CRYPTO_SHA3_256_DIGEST_LEN (32U)

/** @brief Digest length for SHA3-384, in bytes */
#define CRYPTO_SHA3_384_DIGEST_LEN (48U)

/** @brief Digest length for SHA3-512, in bytes */
#define CRYPTO_SHA3_512_DIGEST_LEN (64U)

/** @brief SHAKE128 rate parameter, in bytes */
#define CRYPTO_SHAKE128_RATE 168

/** @brief SHAKE256 rate parameter, in bytes */
#define CRYPTO_SHAKE256_RATE 136

/**
 *  @brief      Status codes for hash operations.
 *
 *              These values are returned by hash functions to indicate
 *              the result of the operation. A successful operation returns 
 *              CRYPTO_HASH_STAT_OK, while errors return specific negative
 *              error codes.
 */
typedef enum {
    /* The function was executed successfully */
    CRYPTO_HASH_STAT_OK = 0,
    /* The function was not executed successfully */
    CRYPTO_HASH_STAT_ERROR = -100,
    /* 512MiB max length exceeded */
    CRYPTO_HASH_STAT_LENGTH_TOO_LARGE = -103,
    /* An input pointer was NULL. Operation not performed. */
    CRYPTO_HASH_STAT_NULL_INPUT = -104
} crypto_hash_stat_t;

/**
 *  @brief      SHA-224/256 Context object
 *
 *              The application must not access any member variables
 *              of this structure!
 */
typedef struct {
    /** Counter for bits processed, only 2^32 bits (512 MiBytes) supported */
    uint32_t bitsProcessed;
    union {
        struct {
            /** Byte offset into Ws, used to load message */
            int8_t offsetWb;
            /** Reserved field */
            uint8_t reserved1;
            /** Size of output digest */
            uint8_t digestSize;
            /** Reserved field */
            uint8_t reserved2;
        };
        /** Combined settings used to make initialization more efficient. */
        uint32_t quickSettings;
    };
    /** Internal holding buffer for intermediate and final digest */
    uint32_t digest32[8];
    /** Internal message schedule, indexed by (s & 0xF)
     *  where s = 63 - t, and t is NIST 180-4's message schedule index
     */
    uint32_t Ws32[16];     
} crypto_sha256_ctx_t;

/**
 *  @brief      SHA-384/512 Context object
 *
 *              The application must not access any member variables
 *              of this structure!
 */
typedef struct {
    /** Counter for bits processed, only 2^32 bits (512 MiBytes) supported */
    uint32_t bitsProcessed;
    union {
        struct {
            /** Byte offset into Ws, used to load message */
            int8_t offsetWb;
            /** Reserved field */
            uint8_t reserved1;
            /** Size of output digest */
            uint8_t digestSize;
            /** Reserved field */
            uint8_t reserved2;
        };
        /** Combined settings used to make initialization more efficient. */
        uint32_t quickSettings;
    };
    /** Internal holding buffer for intermediate and final digest */
    uint64_t digest64[8];  // Holds intermediate/final digest
    /** Internal message schedule, indexed by (s & 0xF)
     *  where s = 63 - t, and t is NIST 180-4's message schedule index
     */
    uint64_t Ws64[16];
} crypto_sha512_ctx_t;

/*!
 *  @brief SHA3-224/256/384/512 Context object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    /** Internal storage for current SHA3 state */
    union {
        /** Byte-wise access to state */
        uint8_t b[200];
        /** Long-word access to state */
        uint64_t q[25];
    } state;
    /** Internal pt value */
    size_t pt;
    /** Internal rsize value */
    size_t rsize;
    /** Digest length */
    size_t mdlen;
} crypto_sha3_ctx_t;

/*!
 *  @brief SHAKE128/256 Context object (alias to SHA3 object)
 *
 *  The application must not access any member variables of this structure!
 */
typedef crypto_sha3_ctx_t crypto_shake_ctx_t;

/* SHA-224 Function Implementations */

/**
 *  @brief      Compute the SHA-224 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 28 bytes / 3 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA224(const void *data, size_t len, 
                                        uint32_t *md);

/**
 *  @brief      Init a SHA-224 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA224, SHA-256 share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA224_init(crypto_sha256_ctx_t *ctx);

/**
 *  @brief      Update a SHA-224 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA224, SHA-256 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
#define Crypto_SHA224_update(ctx, data, len) \
        Crypto_SHA256_update(ctx, data, len)

/**
 *  @brief      Finalize a SHA-224 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA224, SHA-256 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 28 bytes / 3 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
#define Crypto_SHA224_final(ctx, md) \
        Crypto_SHA256_final(ctx, md)

/* SHA-256 Function Implementations */

/**
 *  @brief      Compute the SHA-256 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 32 bytes / 4 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA256(const void *data, size_t len, 
                                        uint32_t *md);

/**
 *  @brief      Init a SHA-256 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA224, SHA-256 share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA256_init(crypto_sha256_ctx_t *ctx);

/**
 *  @brief      Update a SHA-256 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA224, SHA-256 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA256_update(crypto_sha256_ctx_t *ctx, \
                                               const void *data, size_t len);

/**
 *  @brief      Finalize a SHA-256 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA224, SHA-256 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 32 bytes / 8 words
 *
 *  @return     The resulting status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA256_final(crypto_sha256_ctx_t *ctx, 
                                              uint32_t *md);

/* SHA-384 Function Implementations */

/**
 *  @brief      Compute the SHA-384 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 48 bytes / 12 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA384(const void *data, size_t len, 
                                        uint32_t *md);

/**
 *  @brief      Init a SHA-384 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA-384, SHA-512 share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA384_init(crypto_sha512_ctx_t *ctx);

/**
 *  @brief      Update a SHA-384 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA-384, SHA-512 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
#define Crypto_SHA384_update(ctx, data, len) \
        Crypto_SHA512_update(ctx, data, len)

/**
 *  @brief      Finalize a SHA-384 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA-384, SHA-512 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 48 bytes / 12 words
 *
 *  @return     The resulting status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
#define Crypto_SHA384_final(ctx, md) \
        Crypto_SHA512_final(ctx, md)

/* SHA-512 Function Implementations */

/**
 *  @brief      Compute the SHA-512 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 64 bytes / 16 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA512(const void *data, size_t len, 
                                        uint32_t *md);

/**
 *  @brief      Init a SHA-512 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA-384, SHA-512 share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA512_init(crypto_sha512_ctx_t *ctx);

/**
 *  @brief      Update a SHA-512 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA-384, SHA-512 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA512_update(crypto_sha512_ctx_t *ctx, \
                                    const void *data, size_t len);

/**
 *  @brief      Finalize a SHA-512 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA-384, SHA-512 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 64 bytes / 16 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 *  @retval     CRYPTO_HASH_STAT_LENGTH_TOO_LARGE         
 *  @retval     CRYPTO_HASH_STAT_NULL_INPUT
 */
extern crypto_hash_stat_t Crypto_SHA512_final(crypto_sha512_ctx_t *ctx, 
                                              uint32_t *md);

/* SHA3-224 Function Implementations */

/**
 *  @brief      Compute the SHA3-224 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 28 bytes / 7 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_224(const void *data, size_t len, 
                                          uint32_t *md);

/**
 *  @brief      Init a SHA3-224 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA3 hashes all share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_224_init(crypto_sha3_ctx_t *ctx);

/**
 *  @brief      Update a SHA3-224 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA3 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_224_update(ctx, data, len) \
        Crypto_SHA3_update(ctx, data, len)

/**
 *  @brief      Finalize a SHA3-224 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA3 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 28 bytes / 7 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_224_final(ctx, md) \
        Crypto_SHA3_final(ctx, md)

/* SHA3-256 Function Implementations */

/**
 *  @brief      Compute the SHA3-256 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 32 bytes / 8 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_256(const void *data, size_t len, 
                                          uint32_t *md);

/**
 *  @brief      Init a SHA3-256 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA3 hashes all share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_256_init(crypto_sha3_ctx_t *ctx);

/**
 *  @brief      Update a SHA3-256 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA3 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_256_update(ctx, data, len) \
        Crypto_SHA3_update(ctx, data, len)

/**
 *  @brief      Finalize a SHA3-256 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA3 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 32 bytes / 8 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_256_final(ctx, md) \
        Crypto_SHA3_final(ctx, md)

/* SHA3-384 Function Implementations */

/**
 *  @brief      Compute the SHA3-384 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 48 bytes / 12 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_384(const void *data, size_t len, uint32_t *md);

/**
 *  @brief      Init a SHA3-384 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA3 hashes all share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_384_init(crypto_sha3_ctx_t *ctx);

/**
 *  @brief      Update a SHA3-384 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA3 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_384_update(ctx, data, len) \
        Crypto_SHA3_update(ctx, data, len)

/**
 *  @brief      Finalize a SHA3-384 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA3 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 48 bytes / 12 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_384_final(ctx, md) \
        Crypto_SHA3_final(ctx, md)

/* SHA3-512 Function Implementations */

/**
 *  @brief      Compute the SHA3-512 hash for a byte string in one shot.
 *
 *  @param[in]  data           Pointer to the first element of data to hash
 *  @param[in]  len            Number of bytes pointed to by data to hash
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 64 bytes / 16 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_512(const void *data, size_t len, \
                                          uint32_t *md);

/**
 *  @brief      Init a SHA3-512 hash context for hash streaming mode.
 *              Call this function before _update, _final.
 *              NOTE: the SHA3 hashes all share the same context structure.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */
extern crypto_hash_stat_t Crypto_SHA3_512_init(crypto_sha3_ctx_t *ctx);

/**
 *  @brief      Update a SHA3-512 hash context with new data in streaming mode.
 *              Call this function after _init and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: the SHA3 _update functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_512_update(ctx, data, len) \
        Crypto_SHA3_update(ctx, data, len)

/**
 *  @brief      Finalize a SHA3-512 hash in hash streaming mode and save the
 *              message digest.  Call this function after _init, _update.
 *              NOTE: the SHA3 _final functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest;
 *                             this buffer must be at least 64 bytes / 16 words
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHA3_512_final(ctx, md) \
        Crypto_SHA3_final(ctx, md)

/**
 *  @brief      Update a SHA3 hash context with new data in streaming mode.
 *              Call this function after _update and before _final.
 *              This function may be called multiple times to add more data
 *              before finally calling _final.
 *              NOTE: This function is aliased to and not called directly.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[in]  data           Pointer to the first byte to digest
 *  @param[in]  len            Number of bytes to digest
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
extern crypto_hash_stat_t Crypto_SHA3_update(crypto_sha3_ctx_t *ctx, \
                                             const void *data, size_t len);
               
/**
 *  @brief      Finalize a SHA3 hash in hash streaming mode and save the
 *              message digest.  
 *              NOTE: This function is aliased to and not called directly.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *  @param[out] md             Pointer to the location to store the msg digest.
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */                                             
extern crypto_hash_stat_t Crypto_SHA3_final(crypto_sha3_ctx_t *ctx, \
                                            uint32_t *md);

/* SHAKE128 Function Implementations */

/**
 *  @brief      Run a complete SHAKE128 operation with absorb and squeeze
 *              in one shot.
 *
 *  @param[out] out            Pointer to the location to place output data
 *  @param[in]  outLen         Number of bytes of output data to squeeze
 *  @param[in]  in             Pointer to the input data to absorb
 *  @param[in]  inLen          Number of bytes of input data to absorb
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
extern crypto_hash_stat_t Crypto_SHAKE128(uint8_t *out, size_t outLen, \
                                          const uint8_t *in, size_t inLen);

/**
 *  @brief      Init a SHAKE128 XOF context.
 *              Call this function before _absorb, _xof, _squeeze.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
extern crypto_hash_stat_t Crypto_SHAKE128_init(crypto_shake_ctx_t *ctx);

/**
 *  @brief      Absorb data into a SHAKE128 XOF context.
 *              Call this function after _init and before _xof.
 *              This function may be called multiple times to add more data
 *              before finally calling _xof.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *  @param[in]  data           Pointer to the first byte to absorb
 *  @param[in]  len            Number of bytes to absorb
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHAKE128_absorb(ctx, data, len) \
        Crypto_SHA3_update(ctx, data, len)

/**
 *  @brief      Switch a SHAKE128 XOF context from absorb to squeeze (XOF).
 *              Call this function after _absorb and before _squeeze.
 *              NOTE: The SHAKE _xof functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
#define Crypto_SHAKE128_xof(ctx) Crypto_SHAKE_xof(ctx)

/**
 *  @brief      Squeeze bytes out from SHAKE128 XOF context.
 *              Call this function after _xof.
 *              NOTE: The SHAKE _squeeze functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *  @param[out] data           Pointer to location to store output
 *  @param[in]  len            Number of bytes to squeeze
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
#define Crypto_SHAKE128_squeeze(ctx, data, len) \
        Crypto_SHAKE_squeeze(ctx, data, len)


/**
 *  @brief      Run a complete SHAKE256 operation with absorb and squeeze
 *              in one shot.
 *
 *  @param[out] out            Pointer to the location to place output data
 *  @param[in]  outLen         Number of bytes of output data to squeeze
 *  @param[in]  in             Pointer to the input data to absorb
 *  @param[in]  inLen          Number of bytes of input data to absorb
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
 extern crypto_hash_stat_t Crypto_SHAKE256(uint8_t *out, size_t outLen, \
                                           const uint8_t *in, size_t inLen);

/**
 *  @brief      Init a SHAKE256 XOF context.
 *              Call this function before _absorb, _xof, _squeeze.
 *
 *  @param[in]  ctx            Pointer to the hash context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
extern crypto_hash_stat_t Crypto_SHAKE256_init(crypto_shake_ctx_t *ctx);

/**
 *  @brief      Absorb data into a SHAKE256 XOF context.
 *              Call this function after _init and before _xof.
 *              This function may be called multiple times to add more data
 *              before finally calling _xof.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *  @param[in]  data           Pointer to the first byte to absorb
 *  @param[in]  len            Number of bytes to absorb
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK
 */
#define Crypto_SHAKE256_absorb(ctx, data, len) \
        Crypto_SHA3_update(ctx, data, len)

/**
 *  @brief      Switch a SHAKE256 XOF context from absorb to squeeze (XOF).
 *              Call this function after _absorb.
 *              NOTE: The SHAKE _xof functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
#define Crypto_SHAKE256_xof(ctx) \
        Crypto_SHAKE_xof(ctx)

/**
 *  @brief      Squeeze bytes out from SHAKE256 XOF context.
 *              Call this function after _xof.
 *              NOTE: the SHAKE _squeeze functions are aliased.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *  @param[out] data           Pointer to location to store output
 *  @param[in]  len            Number of bytes to squeeze
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
#define Crypto_SHAKE256_squeeze(ctx, data, len) \
        Crypto_SHAKE_squeeze(ctx, data, len)

/**
 *  @brief      Switch a SHAKE XOF context from absorb to squeeze (XOF).
 *              NOTE: This function is aliased to and not called directly.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
extern crypto_hash_stat_t Crypto_SHAKE_xof(crypto_shake_ctx_t *ctx);

/**
 *  @brief      Squeeze bytes out from SHAKE XOF context.
 *              NOTE: This function is aliased to and not called directly.
 *
 *  @param[in]  ctx            Pointer to the XOF context
 *  @param[out] data           Pointer to location to store output
 *  @param[in]  len            Number of bytes to squeeze to output
 *
 *  @return     The status of the operation, one of crypto_hash_stat_t.
 *  @retval     CRYPTO_HASH_STAT_OK if the hash operation succeeded.
 *  @retval     CRYPTO_HASH_STAT_ERROR if the hash operation failed.
 */                                       
extern crypto_hash_stat_t Crypto_SHAKE_squeeze(crypto_shake_ctx_t *ctx, \
                                               void *data, size_t len);

#endif /* CRYPTO_SHA_H_ */
