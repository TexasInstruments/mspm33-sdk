/*
 * MSPM33 AES Hardware Accelerator (AESADV) AEAD Operation Context
 *
 * Copyright (c) 2026, Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TI_MSPM33_AES_AEAD_CTX_H
#define TI_MSPM33_AES_AEAD_CTX_H

#include <stdint.h>
#include <stddef.h>

#include "ti_mspm33_aes_ctx.h"

typedef struct {
    void *aesadv;
    psa_algorithm_t alg;
    uint8_t key_material[TI_MSPM33_AES_MAX_KEY_SIZE];
    size_t key_size;
    uint8_t nonce[16];
    size_t nonce_length;
    size_t aad_length;
    size_t plaintext_length;
    uint8_t direction;
    uint8_t nonce_set;
    uint8_t lengths_set;
    uint8_t active;
    uint8_t aad_buffer[TI_MSPM33_AES_BLOCK_SIZE];
    size_t aad_buffer_length;
    uint8_t pt_buffer[TI_MSPM33_AES_BLOCK_SIZE];
    size_t pt_buffer_length;
} ti_mspm33_aead_operation_t;

#endif /* TI_MSPM33_AES_AEAD_CTX_H */
