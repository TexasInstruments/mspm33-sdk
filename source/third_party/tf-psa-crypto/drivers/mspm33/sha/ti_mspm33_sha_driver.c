/*
 * MSPM33 SHA Hardware Accelerator (SHAW) Driver Implementation
 *
 * Copyright (c) 2026, Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <psa/crypto.h>
#include "ti_mspm33_sha_driver.h"
#include <ti/driverlib/m33/dl_shadrv.h>
#include "mbedtls/constant_time.h"

#define SHAW_HW(op) ((SHAW_Regs *)((op)->shaw))

static psa_algorithm_t ti_mspm33_get_hmac_base_alg(psa_algorithm_t alg)
{
    return PSA_ALG_GET_HASH(alg);
}

/* ============================================================================
 * Hash Single-Shot Implementation
 * ============================================================================ */

psa_status_t ti_mspm33_transparent_hash_compute(
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    ti_mspm33_hash_operation_t operation = {0};
    psa_status_t status;

    status = ti_mspm33_transparent_hash_setup(&operation, alg, input_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = ti_mspm33_transparent_hash_update(&operation, input, input_length);
    if (status != PSA_SUCCESS) {
        ti_mspm33_transparent_hash_abort(&operation);
        return status;
    }

    status = ti_mspm33_transparent_hash_finish(&operation, hash, hash_size, hash_length);
    return status;
}

/* ============================================================================
 * Hash Multipart Implementation
 * ============================================================================ */

psa_status_t ti_mspm33_transparent_hash_setup(
    ti_mspm33_hash_operation_t *operation,
    psa_algorithm_t alg,
    size_t total_length)
{
    DL_SHAW_ALGORITHM shaw_alg;
    size_t digest_size;
    DL_SHAW_STATUS dl_status;

    if (operation->active != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    switch (alg) {
        case PSA_ALG_SHA_256:
            shaw_alg = DL_SHAW_ALGORITHM_SHA_256;
            digest_size = TI_MSPM33_SHA256_DIGEST_SIZE;
            break;
        case PSA_ALG_SHA_224:
            shaw_alg = DL_SHAW_ALGORITHM_SHA_224;
            digest_size = TI_MSPM33_SHA224_DIGEST_SIZE;
            break;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
    }

    operation->shaw = SHAW;

    dl_status = DL_SHAW_init(SHAW_HW(operation));
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    dl_status = DL_SHAW_setModeConfig(SHAW_HW(operation), shaw_alg, DL_SHAW_MODE_HASH);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    if (total_length > 0) {
        dl_status = DL_SHAW_setDataLength(SHAW_HW(operation), total_length, DL_SHAW_MODE_HASH);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }
        DL_SHAW_signalModeAndLengthAvailable(SHAW_HW(operation));
    } else {
        DL_SHAW_signalModeAvailable(SHAW_HW(operation));
    }

    operation->algorithm = (uint32_t)shaw_alg;
    operation->buffer_length = 0;
    operation->total_length = 0;
    operation->digest_size = digest_size;
    operation->active = 1;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_transparent_hash_clone(
    const ti_mspm33_hash_operation_t *source_operation,
    ti_mspm33_hash_operation_t *target_operation)
{
    (void) source_operation;
    (void) target_operation;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t ti_mspm33_transparent_hash_update(
    ti_mspm33_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    size_t combined_length;
    DL_SHAW_STATUS dl_status;

    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    if (input_length == 0) {
        return PSA_SUCCESS;
    }

    combined_length = operation->buffer_length + input_length;

    while (combined_length >= TI_MSPM33_SHA_BLOCK_SIZE) {
        size_t bytes_from_buffer = TI_MSPM33_SHA_BLOCK_SIZE - operation->buffer_length;
        uint8_t block[TI_MSPM33_SHA_BLOCK_SIZE];

        if (operation->buffer_length > 0) {
            memcpy(block, operation->buffer, operation->buffer_length);
            memcpy(block + operation->buffer_length, input, bytes_from_buffer);
            input += bytes_from_buffer;
            input_length -= bytes_from_buffer;
            operation->buffer_length = 0;
        } else {
            memcpy(block, input, TI_MSPM33_SHA_BLOCK_SIZE);
            input += TI_MSPM33_SHA_BLOCK_SIZE;
            input_length -= TI_MSPM33_SHA_BLOCK_SIZE;
        }

        dl_status = DL_SHAW_writeDataBlock(SHAW_HW(operation), (uint32_t *)block, TI_MSPM33_SHA_BLOCK_SIZE);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }

        dl_status = DL_SHAW_signalInterimDataAvailable(SHAW_HW(operation));
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }

        combined_length -= TI_MSPM33_SHA_BLOCK_SIZE;
    }

    if (input_length > 0) {
        memcpy(operation->buffer + operation->buffer_length, input, input_length);
        operation->buffer_length += input_length;
    }

    operation->total_length += input_length;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_transparent_hash_finish(
    ti_mspm33_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    DL_SHAW_STATUS dl_status;
    uint32_t numWords;
    uint32_t numBytes;

    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    if (hash_size < operation->digest_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    numWords = operation->buffer_length / sizeof(uint32_t);
    numBytes = operation->buffer_length % sizeof(uint32_t);

    if (operation->buffer_length > 0) {
        dl_status = DL_SHAW_writePartialBlock(SHAW_HW(operation), (uint32_t *)operation->buffer, numWords, numBytes);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }
    }

    if (operation->total_length == 0) {
        size_t total_msg_length = operation->buffer_length;
        dl_status = DL_SHAW_setDataLength(SHAW_HW(operation), total_msg_length, DL_SHAW_MODE_HASH);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }
        DL_SHAW_signalLengthAvailable(SHAW_HW(operation));
    }

    dl_status = DL_SHAW_signalLastDataAvailable(SHAW_HW(operation));
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    while (!DL_SHAW_isOutputReady(SHAW_HW(operation))) {
        __NOP();
    }

    dl_status = DL_SHAW_readDigest(SHAW_HW(operation), (uint32_t *)hash, operation->digest_size);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    DL_SHAW_releaseDigest(SHAW_HW(operation));

    operation->active = 0;
    *hash_length = operation->digest_size;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_transparent_hash_abort(
    ti_mspm33_hash_operation_t *operation)
{
    if (operation->active == 0) {
        return PSA_SUCCESS;
    }

    memset(operation->buffer, 0, TI_MSPM33_SHA_BLOCK_SIZE);
    operation->buffer_length = 0;
    operation->total_length = 0;

    DL_SHAW_close(SHAW_HW(operation));

    operation->active = 0;

    return PSA_SUCCESS;
}

/* ============================================================================
 * MAC Single-Shot Implementation
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
    size_t *mac_length)
{
    ti_mspm33_mac_operation_t operation = {0};
    psa_status_t status;

    status = ti_mspm33_transparent_mac_sign_setup(&operation, attributes, key_buffer, key_buffer_size, alg, input_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = ti_mspm33_transparent_mac_update(&operation, input, input_length);
    if (status != PSA_SUCCESS) {
        ti_mspm33_transparent_mac_abort(&operation);
        return status;
    }

    status = ti_mspm33_transparent_mac_sign_finish(&operation, mac, mac_size, mac_length);
    return status;
}

/* ============================================================================
 * MAC Multipart Implementation
 * ============================================================================ */

psa_status_t ti_mspm33_transparent_mac_sign_setup(
    ti_mspm33_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    size_t total_length)
{
    psa_algorithm_t base_alg;
    DL_SHAW_ALGORITHM shaw_alg;
    size_t digest_size;
    DL_SHAW_STATUS dl_status;

    (void) attributes;

    if (operation->active != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    if (key_buffer_size == 0 || key_buffer_size > TI_MSPM33_MAX_HMAC_KEY_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    base_alg = ti_mspm33_get_hmac_base_alg(alg);

    switch (base_alg) {
        case PSA_ALG_SHA_256:
            shaw_alg = DL_SHAW_ALGORITHM_SHA_256;
            digest_size = TI_MSPM33_SHA256_DIGEST_SIZE;
            break;
        case PSA_ALG_SHA_224:
            shaw_alg = DL_SHAW_ALGORITHM_SHA_224;
            digest_size = TI_MSPM33_SHA224_DIGEST_SIZE;
            break;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
    }

    memcpy(operation->key_material, key_buffer, key_buffer_size);
    operation->key_size = key_buffer_size;

    operation->shaw = SHAW;

    dl_status = DL_SHAW_init(SHAW_HW(operation));
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        memset(operation->key_material, 0, TI_MSPM33_MAX_HMAC_KEY_SIZE);
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    dl_status = DL_SHAW_setModeConfig(SHAW_HW(operation), shaw_alg, DL_SHAW_MODE_HMAC);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        memset(operation->key_material, 0, TI_MSPM33_MAX_HMAC_KEY_SIZE);
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    DL_SHAW_signalModeAvailable(SHAW_HW(operation));

    dl_status = DL_SHAW_writeDataBlock(SHAW_HW(operation), (uint32_t *)operation->key_material, key_buffer_size);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        memset(operation->key_material, 0, TI_MSPM33_MAX_HMAC_KEY_SIZE);
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    dl_status = DL_SHAW_signalDataAvailable(SHAW_HW(operation));
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        memset(operation->key_material, 0, TI_MSPM33_MAX_HMAC_KEY_SIZE);
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    while (!DL_SHAW_isInputReady(SHAW_HW(operation))) {
        __NOP();
    }

    if (total_length > 0) {
        dl_status = DL_SHAW_setDataLength(SHAW_HW(operation), total_length, DL_SHAW_MODE_HMAC);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            memset(operation->key_material, 0, TI_MSPM33_MAX_HMAC_KEY_SIZE);
            return PSA_ERROR_HARDWARE_FAILURE;
        }
        DL_SHAW_signalLengthAvailable(SHAW_HW(operation));
    }

    operation->algorithm = (uint32_t)shaw_alg;
    operation->buffer_length = 0;
    operation->total_length = 0;
    operation->digest_size = digest_size;
    operation->active = 1;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_transparent_mac_verify_setup(
    ti_mspm33_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    size_t total_length)
{
    return ti_mspm33_transparent_mac_sign_setup(operation, attributes, key_buffer, key_buffer_size, alg, total_length);
}

psa_status_t ti_mspm33_transparent_mac_update(
    ti_mspm33_mac_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    size_t combined_length;
    DL_SHAW_STATUS dl_status;

    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    if (input_length == 0) {
        return PSA_SUCCESS;
    }

    combined_length = operation->buffer_length + input_length;

    while (combined_length >= TI_MSPM33_SHA_BLOCK_SIZE) {
        size_t bytes_from_buffer = TI_MSPM33_SHA_BLOCK_SIZE - operation->buffer_length;
        uint8_t block[TI_MSPM33_SHA_BLOCK_SIZE];

        if (operation->buffer_length > 0) {
            memcpy(block, operation->buffer, operation->buffer_length);
            memcpy(block + operation->buffer_length, input, bytes_from_buffer);
            input += bytes_from_buffer;
            input_length -= bytes_from_buffer;
            operation->buffer_length = 0;
        } else {
            memcpy(block, input, TI_MSPM33_SHA_BLOCK_SIZE);
            input += TI_MSPM33_SHA_BLOCK_SIZE;
            input_length -= TI_MSPM33_SHA_BLOCK_SIZE;
        }

        dl_status = DL_SHAW_writeDataBlock(SHAW_HW(operation), (uint32_t *)block, TI_MSPM33_SHA_BLOCK_SIZE);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }

        dl_status = DL_SHAW_signalInterimDataAvailable(SHAW_HW(operation));
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }

        combined_length -= TI_MSPM33_SHA_BLOCK_SIZE;
    }

    if (input_length > 0) {
        memcpy(operation->buffer + operation->buffer_length, input, input_length);
        operation->buffer_length += input_length;
    }

    operation->total_length += input_length;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_transparent_mac_sign_finish(
    ti_mspm33_mac_operation_t *operation,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    DL_SHAW_STATUS dl_status;
    uint32_t numWords;
    uint32_t numBytes;
    uint8_t inner_digest[TI_MSPM33_SHA256_DIGEST_SIZE];
    uint8_t outer_input[TI_MSPM33_SHA_BLOCK_SIZE + TI_MSPM33_SHA256_DIGEST_SIZE];
    size_t i;

    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    if (mac_size < operation->digest_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    numWords = operation->buffer_length / sizeof(uint32_t);
    numBytes = operation->buffer_length % sizeof(uint32_t);

    if (operation->buffer_length > 0) {
        dl_status = DL_SHAW_writePartialBlock(SHAW_HW(operation), (uint32_t *)operation->buffer, numWords, numBytes);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }
    }

    if (operation->total_length == 0) {
        size_t total_msg_length = operation->buffer_length;
        dl_status = DL_SHAW_setDataLength(SHAW_HW(operation), total_msg_length, DL_SHAW_MODE_HMAC);
        if (dl_status != DL_SHAW_STATUS_SUCCESS) {
            return PSA_ERROR_HARDWARE_FAILURE;
        }
        DL_SHAW_signalLengthAvailable(SHAW_HW(operation));
    }

    dl_status = DL_SHAW_signalLastDataAvailable(SHAW_HW(operation));
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    while (!DL_SHAW_isOutputReady(SHAW_HW(operation))) {
        __NOP();
    }

    dl_status = DL_SHAW_readDigest(SHAW_HW(operation), (uint32_t *)inner_digest, operation->digest_size);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    DL_SHAW_releaseDigest(SHAW_HW(operation));

    dl_status = DL_SHAW_setModeConfig(SHAW_HW(operation), (DL_SHAW_ALGORITHM)operation->algorithm, DL_SHAW_MODE_HMAC);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    DL_SHAW_signalModeAvailable(SHAW_HW(operation));

    for (i = 0; i < TI_MSPM33_SHA_BLOCK_SIZE; i++) {
        if (i < operation->key_size) {
            outer_input[i] = operation->key_material[i] ^ 0x5CU;
        } else {
            outer_input[i] = 0x5CU;
        }
    }

    memcpy(outer_input + TI_MSPM33_SHA_BLOCK_SIZE, inner_digest, operation->digest_size);

    dl_status = DL_SHAW_writeDataBlock(SHAW_HW(operation), (uint32_t *)outer_input, TI_MSPM33_SHA_BLOCK_SIZE);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    dl_status = DL_SHAW_signalDataAvailable(SHAW_HW(operation));
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    while (!DL_SHAW_isInputReady(SHAW_HW(operation))) {
        __NOP();
    }

    dl_status = DL_SHAW_setDataLength(SHAW_HW(operation), TI_MSPM33_SHA_BLOCK_SIZE + operation->digest_size, DL_SHAW_MODE_HMAC);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    DL_SHAW_signalLengthAvailable(SHAW_HW(operation));

    numWords = operation->digest_size / sizeof(uint32_t);
    numBytes = operation->digest_size % sizeof(uint32_t);

    dl_status = DL_SHAW_writePartialBlock(SHAW_HW(operation), (uint32_t *)(outer_input + TI_MSPM33_SHA_BLOCK_SIZE), numWords, numBytes);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    dl_status = DL_SHAW_signalLastDataAvailable(SHAW_HW(operation));
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    while (!DL_SHAW_isOutputReady(SHAW_HW(operation))) {
        __NOP();
    }

    dl_status = DL_SHAW_readDigest(SHAW_HW(operation), (uint32_t *)mac, operation->digest_size);
    if (dl_status != DL_SHAW_STATUS_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    DL_SHAW_releaseDigest(SHAW_HW(operation));

    operation->active = 0;
    *mac_length = operation->digest_size;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_transparent_mac_verify_finish(
    ti_mspm33_mac_operation_t *operation,
    const uint8_t *mac,
    size_t mac_length)
{
    uint8_t computed_mac[TI_MSPM33_SHA256_DIGEST_SIZE];
    size_t computed_mac_length;
    psa_status_t status;

    status = ti_mspm33_transparent_mac_sign_finish(operation, computed_mac, sizeof(computed_mac), &computed_mac_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (mac_length != computed_mac_length) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    if (mbedtls_ct_memcmp(mac, computed_mac, mac_length) != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_transparent_mac_abort(
    ti_mspm33_mac_operation_t *operation)
{
    if (operation->active == 0) {
        return PSA_SUCCESS;
    }

    memset(operation->key_material, 0, TI_MSPM33_MAX_HMAC_KEY_SIZE);
    operation->key_size = 0;
    memset(operation->buffer, 0, TI_MSPM33_SHA_BLOCK_SIZE);
    operation->buffer_length = 0;
    operation->total_length = 0;

    DL_SHAW_close(SHAW_HW(operation));

    operation->active = 0;

    return PSA_SUCCESS;
}
