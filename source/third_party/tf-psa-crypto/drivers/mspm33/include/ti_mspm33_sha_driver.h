/*
 * MSPM33 SHA Hardware Accelerator (SHAW) Driver Entry Points
 *
 * Copyright (c) 2026, Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TI_MSPM33_SHA_DRIVER_H
#define TI_MSPM33_SHA_DRIVER_H

#include <psa/crypto.h>
#include "ti_mspm33_sha_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Hash Single-Shot Entry Points
 * ============================================================================ */

psa_status_t ti_mspm33_transparent_hash_compute(
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

/* ============================================================================
 * Hash Multipart Entry Points
 * ============================================================================ */

psa_status_t ti_mspm33_transparent_hash_setup(
    ti_mspm33_hash_operation_t *operation,
    psa_algorithm_t alg,
    size_t total_length);

psa_status_t ti_mspm33_transparent_hash_clone(
    const ti_mspm33_hash_operation_t *source_operation,
    ti_mspm33_hash_operation_t *target_operation);

psa_status_t ti_mspm33_transparent_hash_update(
    ti_mspm33_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t ti_mspm33_transparent_hash_finish(
    ti_mspm33_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t ti_mspm33_transparent_hash_abort(
    ti_mspm33_hash_operation_t *operation);

/* ============================================================================
 * MAC Single-Shot Entry Points
 * ============================================================================ */

psa_status_t ti_mspm33_transparent_mac_compute(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length);

/* ============================================================================
 * MAC Multipart Entry Points (HMAC)
 * ============================================================================ */

psa_status_t ti_mspm33_transparent_mac_sign_setup(
    ti_mspm33_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    size_t total_length);

psa_status_t ti_mspm33_transparent_mac_verify_setup(
    ti_mspm33_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    size_t total_length);

psa_status_t ti_mspm33_transparent_mac_update(
    ti_mspm33_mac_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t ti_mspm33_transparent_mac_sign_finish(
    ti_mspm33_mac_operation_t *operation,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length);

psa_status_t ti_mspm33_transparent_mac_verify_finish(
    ti_mspm33_mac_operation_t *operation,
    const uint8_t *mac,
    size_t mac_length);

psa_status_t ti_mspm33_transparent_mac_abort(
    ti_mspm33_mac_operation_t *operation);

#ifdef __cplusplus
}
#endif

#endif /* TI_MSPM33_SHA_DRIVER_H */
