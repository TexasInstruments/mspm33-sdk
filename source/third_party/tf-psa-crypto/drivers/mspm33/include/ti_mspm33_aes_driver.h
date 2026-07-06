/*
 * MSPM33 AES Hardware Accelerator (AESADV) Driver Entry Points
 *
 * Copyright (c) 2026, Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TI_MSPM33_AES_DRIVER_H
#define TI_MSPM33_AES_DRIVER_H

#include <psa/crypto.h>
#include "ti_mspm33_aes_ctx.h"
#include "ti_mspm33_aes_aead_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Cipher Single-Shot Entry Points
 * Supported algorithms: PSA_ALG_ECB_NO_PADDING, PSA_ALG_CBC_NO_PADDING,
 *                       PSA_ALG_CTR
 * Supported key sizes:  128-bit, 256-bit
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_cipher_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *iv,
    size_t iv_length,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t ti_mspm33_aes_transparent_cipher_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

/* ============================================================================
 * Cipher Multipart Entry Points
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_cipher_encrypt_setup(
    ti_mspm33_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t ti_mspm33_aes_transparent_cipher_decrypt_setup(
    ti_mspm33_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t ti_mspm33_aes_transparent_cipher_set_iv(
    ti_mspm33_cipher_operation_t *operation,
    const uint8_t *iv,
    size_t iv_length);

psa_status_t ti_mspm33_aes_transparent_cipher_update(
    ti_mspm33_cipher_operation_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t ti_mspm33_aes_transparent_cipher_finish(
    ti_mspm33_cipher_operation_t *operation,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t ti_mspm33_aes_transparent_cipher_abort(
    ti_mspm33_cipher_operation_t *operation);

/* ============================================================================
 * AEAD Single-Shot Entry Points
 * Supported algorithms: PSA_ALG_GCM (and shortened tag variants),
 *                       PSA_ALG_CCM (and shortened tag variants, tag 4-8 bytes)
 * Supported key sizes:  128-bit, 256-bit
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce,
    size_t nonce_length,
    const uint8_t *additional_data,
    size_t additional_data_length,
    const uint8_t *plaintext,
    size_t plaintext_length,
    uint8_t *ciphertext,
    size_t ciphertext_size,
    size_t *ciphertext_length);

psa_status_t ti_mspm33_aes_transparent_aead_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce,
    size_t nonce_length,
    const uint8_t *additional_data,
    size_t additional_data_length,
    const uint8_t *ciphertext,
    size_t ciphertext_length,
    uint8_t *plaintext,
    size_t plaintext_size,
    size_t *plaintext_length);

/* ============================================================================
 * AEAD Multipart Entry Points
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_encrypt_setup(
    ti_mspm33_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t ti_mspm33_aes_transparent_aead_decrypt_setup(
    ti_mspm33_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t ti_mspm33_aes_transparent_aead_set_nonce(
    ti_mspm33_aead_operation_t *operation,
    const uint8_t *nonce,
    size_t nonce_length);

psa_status_t ti_mspm33_aes_transparent_aead_set_lengths(
    ti_mspm33_aead_operation_t *operation,
    size_t aad_length,
    size_t plaintext_length);

psa_status_t ti_mspm33_aes_transparent_aead_update_ad(
    ti_mspm33_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t ti_mspm33_aes_transparent_aead_update(
    ti_mspm33_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t ti_mspm33_aes_transparent_aead_finish(
    ti_mspm33_aead_operation_t *operation,
    uint8_t *ciphertext_or_plaintext,
    size_t ciphertext_or_plaintext_size,
    size_t *ciphertext_or_plaintext_length,
    uint8_t *tag,
    size_t tag_size,
    size_t *tag_length);

psa_status_t ti_mspm33_aes_transparent_aead_verify(
    ti_mspm33_aead_operation_t *operation,
    uint8_t *plaintext,
    size_t plaintext_size,
    size_t *plaintext_length,
    const uint8_t *tag,
    size_t tag_length);

psa_status_t ti_mspm33_aes_transparent_aead_abort(
    ti_mspm33_aead_operation_t *operation);

#ifdef __cplusplus
}
#endif

#endif /* TI_MSPM33_AES_DRIVER_H */
