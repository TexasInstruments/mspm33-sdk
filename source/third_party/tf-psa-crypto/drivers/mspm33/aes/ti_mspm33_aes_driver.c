/*
 * MSPM33 AES Hardware Accelerator (AESADV) Driver Implementation
 *
 * Copyright (c) 2026, Texas Instruments Incorporated
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <psa/crypto.h>
#include "ti_mspm33_aes_driver.h"
#include "ti_mspm33_aes_aead_ctx.h"
#include <ti/driverlib/dl_aesadv.h>
#include "mbedtls/constant_time.h"

#define AESADV_HW(op) ((AESADV_Regs *)((op)->aesadv))

static void ti_mspm33_aesadv_init_hw(void)
{
    DL_AESADV_reset(AESADV);
    DL_AESADV_enablePower(AESADV);
    DL_AESADV_clearInterruptStatus(
        AESADV, (DL_AESADV_INTERRUPT_INPUT_CONTEXT_READY |
                 DL_AESADV_INTERRUPT_INPUT_READY |
                 DL_AESADV_INTERRUPT_OUTPUT_READY |
                 DL_AESADV_INTERRUPT_SAVED_OUTPUT_CONTEXT_READY));
    DL_AESADV_enableInterrupt(
        AESADV, (DL_AESADV_INTERRUPT_INPUT_CONTEXT_READY |
                 DL_AESADV_INTERRUPT_INPUT_READY |
                 DL_AESADV_INTERRUPT_OUTPUT_READY |
                 DL_AESADV_INTERRUPT_SAVED_OUTPUT_CONTEXT_READY));
}

/* ============================================================================
 * Helpers: Algorithm and Key Size Mapping
 * ============================================================================ */

static psa_status_t _ti_mspm33_map_key_size(
    const psa_key_attributes_t *attributes,
    DL_AESADV_KEY_SIZE *hw_key_size)
{
    size_t key_bits = psa_get_key_bits(attributes);

    switch (key_bits) {
        case 128:
            *hw_key_size = DL_AESADV_KEY_SIZE_128_BIT;
            return PSA_SUCCESS;
        case 256:
            *hw_key_size = DL_AESADV_KEY_SIZE_256_BIT;
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
    }
}

static psa_status_t _ti_mspm33_get_aead_tag_length(
    psa_algorithm_t alg,
    size_t *tag_length)
{
    psa_algorithm_t base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);

    if (base_alg != PSA_ALG_GCM && base_alg != PSA_ALG_CCM) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* PSA_ALG_AEAD_GET_TAG_LENGTH returns tag length in bytes */
    *tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);
    return PSA_SUCCESS;
}

static psa_status_t _ti_mspm33_map_ccm_nonce_to_ctr_width(
    size_t nonce_length,
    DL_AESADV_CCM_CTR_WIDTH *ctr_width)
{
    switch (nonce_length) {
        case 7:
            *ctr_width = DL_AESADV_CCM_CTR_WIDTH_8_BYTES;
            return PSA_SUCCESS;
        case 8:
            *ctr_width = DL_AESADV_CCM_CTR_WIDTH_7_BYTES;
            return PSA_SUCCESS;
        case 9:
            *ctr_width = DL_AESADV_CCM_CTR_WIDTH_6_BYTES;
            return PSA_SUCCESS;
        case 10:
            *ctr_width = DL_AESADV_CCM_CTR_WIDTH_5_BYTES;
            return PSA_SUCCESS;
        case 11:
            *ctr_width = DL_AESADV_CCM_CTR_WIDTH_4_BYTES;
            return PSA_SUCCESS;
        case 12:
            *ctr_width = DL_AESADV_CCM_CTR_WIDTH_3_BYTES;
            return PSA_SUCCESS;
        case 13:
            *ctr_width = DL_AESADV_CCM_CTR_WIDTH_2_BYTES;
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_INVALID_ARGUMENT;
    }
}

static psa_status_t _ti_mspm33_map_tag_length_to_ccm_tag_width(
    size_t tag_length,
    DL_AESADV_CCM_TAG_WIDTH *tag_width)
{
    switch (tag_length) {
        case 4:
            *tag_width = DL_AESADV_CCM_TAG_WIDTH_4_BYTES;
            return PSA_SUCCESS;
        case 5:
            *tag_width = DL_AESADV_CCM_TAG_WIDTH_5_BYTES;
            return PSA_SUCCESS;
        case 6:
            *tag_width = DL_AESADV_CCM_TAG_WIDTH_6_BYTES;
            return PSA_SUCCESS;
        case 7:
            *tag_width = DL_AESADV_CCM_TAG_WIDTH_7_BYTES;
            return PSA_SUCCESS;
        case 8:
            *tag_width = DL_AESADV_CCM_TAG_WIDTH_8_BYTES;
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_INVALID_ARGUMENT;
    }
}

/* ============================================================================
 * Single-Shot Cipher: ECB
 * ============================================================================ */

static psa_status_t ti_mspm33_cipher_ecb_operation(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    DL_AESADV_KEY_SIZE key_size,
    uint8_t direction)
{
    DL_AESADV_Config config = {0};
    size_t offset;

    (void)key_buffer_size;

    if ((input_length % TI_MSPM33_AES_BLOCK_SIZE) != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ti_mspm33_aesadv_init_hw();

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_ECB;
    config.direction = (direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
    config.lowerCryptoLength = input_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initECB(AESADV, &config);

    while (!DL_AESADV_isInputReady(AESADV)) {
        __NOP();
    }

    for (offset = 0; offset < input_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        DL_AESADV_loadInputData(AESADV, input + offset);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output + offset);
    }

    return PSA_SUCCESS;
}

/* ============================================================================
 * Single-Shot Cipher: CBC
 * ============================================================================ */

static psa_status_t ti_mspm33_cipher_cbc_operation(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *iv,
    size_t iv_length,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    DL_AESADV_KEY_SIZE key_size,
    uint8_t direction)
{
    DL_AESADV_Config config = {0};
    size_t offset;

    (void)key_buffer_size;

    if (iv_length != TI_MSPM33_AES_BLOCK_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((input_length % TI_MSPM33_AES_BLOCK_SIZE) != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ti_mspm33_aesadv_init_hw();

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_CBC;
    config.direction = (direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
    config.iv = (uint8_t *)iv;
    config.lowerCryptoLength = input_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initCBC(AESADV, &config);

    while (!DL_AESADV_isInputReady(AESADV)) {
        __NOP();
    }

    for (offset = 0; offset < input_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        DL_AESADV_loadInputData(AESADV, input + offset);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output + offset);
    }

    return PSA_SUCCESS;
}

/* ============================================================================
 * Single-Shot Cipher: CTR
 * ============================================================================ */

static psa_status_t ti_mspm33_cipher_ctr_operation(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *iv,
    size_t iv_length,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    DL_AESADV_KEY_SIZE key_size)
{
    DL_AESADV_Config config = {0};
    size_t offset;
    size_t full_blocks = input_length / TI_MSPM33_AES_BLOCK_SIZE;
    size_t remainder = input_length % TI_MSPM33_AES_BLOCK_SIZE;
    uint8_t block_buf[TI_MSPM33_AES_BLOCK_SIZE];

    (void)key_buffer_size;

    if (iv_length != TI_MSPM33_AES_BLOCK_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ti_mspm33_aesadv_init_hw();

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_CTR;
    config.direction = DL_AESADV_DIR_ENCRYPT;
    config.ctr_ctrWidth = DL_AESADV_CTR_WIDTH_128_BIT;
    config.iv = (uint8_t *)iv;
    config.lowerCryptoLength = input_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initCTR(AESADV, &config);

    while (!DL_AESADV_isInputReady(AESADV)) {
        __NOP();
    }

    /* Process full blocks */
    for (offset = 0; offset < full_blocks; offset++) {
        DL_AESADV_loadInputData(AESADV, input + offset * TI_MSPM33_AES_BLOCK_SIZE);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output + offset * TI_MSPM33_AES_BLOCK_SIZE);
    }

    /* Process final partial block if present */
    if (remainder > 0) {
        memset(block_buf, 0, TI_MSPM33_AES_BLOCK_SIZE);
        memcpy(block_buf, input + full_blocks * TI_MSPM33_AES_BLOCK_SIZE, remainder);

        DL_AESADV_loadInputData(AESADV, block_buf);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, block_buf);

        memcpy(output + full_blocks * TI_MSPM33_AES_BLOCK_SIZE, block_buf, remainder);
    }

    return PSA_SUCCESS;
}

/* ============================================================================
 * Single-Shot Cipher: CFB
 * ============================================================================ */

static psa_status_t ti_mspm33_cipher_cfb_operation(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *iv,
    size_t iv_length,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    DL_AESADV_KEY_SIZE key_size,
    uint8_t direction)
{
    DL_AESADV_Config config = {0};
    size_t full_blocks = input_length / TI_MSPM33_AES_BLOCK_SIZE;
    size_t remainder = input_length % TI_MSPM33_AES_BLOCK_SIZE;
    uint8_t block_buf[TI_MSPM33_AES_BLOCK_SIZE];
    size_t offset;

    (void)key_buffer_size;

    if (iv_length != TI_MSPM33_AES_BLOCK_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ti_mspm33_aesadv_init_hw();

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_CFB;
    config.direction = (direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
    config.cfb_fbWidth = DL_AESADV_FB_WIDTH_128;
    config.iv = (uint8_t *)iv;
    config.lowerCryptoLength = input_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initCFB(AESADV, &config);

    while (!DL_AESADV_isInputReady(AESADV)) {
        __NOP();
    }

    for (offset = 0; offset < full_blocks; offset++) {
        DL_AESADV_loadInputData(AESADV, input + offset * TI_MSPM33_AES_BLOCK_SIZE);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output + offset * TI_MSPM33_AES_BLOCK_SIZE);
    }

    if (remainder > 0) {
        memset(block_buf, 0, TI_MSPM33_AES_BLOCK_SIZE);
        memcpy(block_buf, input + full_blocks * TI_MSPM33_AES_BLOCK_SIZE, remainder);

        DL_AESADV_loadInputData(AESADV, block_buf);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, block_buf);

        memcpy(output + full_blocks * TI_MSPM33_AES_BLOCK_SIZE, block_buf, remainder);
    }

    return PSA_SUCCESS;
}

/* ============================================================================
 * Single-Shot Cipher: OFB
 * ============================================================================ */

static psa_status_t ti_mspm33_cipher_ofb_operation(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *iv,
    size_t iv_length,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    DL_AESADV_KEY_SIZE key_size,
    uint8_t direction)
{
    DL_AESADV_Config config = {0};
    size_t full_blocks = input_length / TI_MSPM33_AES_BLOCK_SIZE;
    size_t remainder = input_length % TI_MSPM33_AES_BLOCK_SIZE;
    uint8_t block_buf[TI_MSPM33_AES_BLOCK_SIZE];
    size_t offset;

    (void)key_buffer_size;

    if (iv_length != TI_MSPM33_AES_BLOCK_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ti_mspm33_aesadv_init_hw();

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_OFB;
    config.direction = (direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
    config.iv = (uint8_t *)iv;
    config.lowerCryptoLength = input_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initOFB(AESADV, &config);

    while (!DL_AESADV_isInputReady(AESADV)) {
        __NOP();
    }

    for (offset = 0; offset < full_blocks; offset++) {
        DL_AESADV_loadInputData(AESADV, input + offset * TI_MSPM33_AES_BLOCK_SIZE);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output + offset * TI_MSPM33_AES_BLOCK_SIZE);
    }

    if (remainder > 0) {
        memset(block_buf, 0, TI_MSPM33_AES_BLOCK_SIZE);
        memcpy(block_buf, input + full_blocks * TI_MSPM33_AES_BLOCK_SIZE, remainder);

        DL_AESADV_loadInputData(AESADV, block_buf);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, block_buf);

        memcpy(output + full_blocks * TI_MSPM33_AES_BLOCK_SIZE, block_buf, remainder);
    }

    return PSA_SUCCESS;
}

/* ============================================================================
 * Cipher Single-Shot Entry Points
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
    size_t *output_length)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_status_t status;

    /* AESADV hardware IP needs to know key size in advance in order to be able to configure 
       the hardware and set the AES key using DL_AESADV_setKey                            */
    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (output_size < input_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (alg == PSA_ALG_ECB_NO_PADDING) {
        status = ti_mspm33_cipher_ecb_operation(
            key_buffer, key_buffer_size, input, input_length,
            output, key_size, 0);
    } else if (alg == PSA_ALG_CBC_NO_PADDING) {
        status = ti_mspm33_cipher_cbc_operation(
            key_buffer, key_buffer_size, iv, iv_length,
            input, input_length, output, key_size, 0);
    } else if (alg == PSA_ALG_CTR) {
        status = ti_mspm33_cipher_ctr_operation(
            key_buffer, key_buffer_size, iv, iv_length,
            input, input_length, output, key_size);
    } else if (alg == PSA_ALG_CFB) {
        status = ti_mspm33_cipher_cfb_operation(
            key_buffer, key_buffer_size, iv, iv_length,
            input, input_length, output, key_size, 0);
    } else if (alg == PSA_ALG_OFB) {
        status = ti_mspm33_cipher_ofb_operation(
            key_buffer, key_buffer_size, iv, iv_length,
            input, input_length, output, key_size, 0);
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (status == PSA_SUCCESS) {
        *output_length = input_length;
    }

    return status;
}

psa_status_t ti_mspm33_aes_transparent_cipher_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_status_t status;
    size_t iv_length = 0;
    size_t ciphertext_length;

    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (alg == PSA_ALG_ECB_NO_PADDING) {
        iv_length = 0;
    } else if (alg == PSA_ALG_CBC_NO_PADDING) {
        iv_length = TI_MSPM33_AES_BLOCK_SIZE;
    } else if (alg == PSA_ALG_CTR) {
        iv_length = TI_MSPM33_AES_BLOCK_SIZE;
    } else if (alg == PSA_ALG_CFB) {
        iv_length = TI_MSPM33_AES_BLOCK_SIZE;
    } else if (alg == PSA_ALG_OFB) {
        iv_length = TI_MSPM33_AES_BLOCK_SIZE;
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    ciphertext_length = input_length - iv_length;

    if (output_size < ciphertext_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (alg == PSA_ALG_ECB_NO_PADDING) {
        if (input_length % TI_MSPM33_AES_BLOCK_SIZE != 0) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        status = ti_mspm33_cipher_ecb_operation(
            key_buffer, key_buffer_size, input, input_length,
            output, key_size, 1);
        if (status == PSA_SUCCESS) {
            *output_length = input_length;
        }
    } else if (alg == PSA_ALG_CBC_NO_PADDING) {
        if (ciphertext_length % TI_MSPM33_AES_BLOCK_SIZE != 0) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        status = ti_mspm33_cipher_cbc_operation(
            key_buffer, key_buffer_size,
            input, iv_length,
            input + iv_length, ciphertext_length,
            output, key_size, 1);
        if (status == PSA_SUCCESS) {
            *output_length = ciphertext_length;
        }
    } else if (alg == PSA_ALG_CTR) {
        if (input_length < TI_MSPM33_AES_BLOCK_SIZE) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        status = ti_mspm33_cipher_ctr_operation(
            key_buffer, key_buffer_size,
            input, iv_length,
            input + iv_length, ciphertext_length,
            output, key_size);
        if (status == PSA_SUCCESS) {
            *output_length = ciphertext_length;
        }
    } else if (alg == PSA_ALG_CFB) {
        if (input_length < TI_MSPM33_AES_BLOCK_SIZE) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        status = ti_mspm33_cipher_cfb_operation(
            key_buffer, key_buffer_size,
            input, iv_length,
            input + iv_length, ciphertext_length,
            output, key_size, 1);
        if (status == PSA_SUCCESS) {
            *output_length = ciphertext_length;
        }
    } else if (alg == PSA_ALG_OFB) {
        if (input_length < TI_MSPM33_AES_BLOCK_SIZE) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        status = ti_mspm33_cipher_ofb_operation(
            key_buffer, key_buffer_size,
            input, iv_length,
            input + iv_length, ciphertext_length,
            output, key_size, 1);
        if (status == PSA_SUCCESS) {
            *output_length = ciphertext_length;
        }
    }

    return status;
}

/* ============================================================================
 * Cipher Multipart: Setup
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_cipher_encrypt_setup(
    ti_mspm33_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_status_t status;

    ti_mspm33_aesadv_init_hw();

    if (operation->active != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (alg != PSA_ALG_ECB_NO_PADDING && alg != PSA_ALG_CBC_NO_PADDING &&
        alg != PSA_ALG_CTR && alg != PSA_ALG_CFB && alg != PSA_ALG_OFB) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (key_buffer_size > TI_MSPM33_AES_MAX_KEY_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    operation->aesadv = AESADV;
    operation->alg = alg;
    operation->direction = 0;
    operation->key_size = key_buffer_size;
    memcpy(operation->key_material, key_buffer, key_buffer_size);
    operation->buffer_length = 0;
    operation->iv_set = 0;
    operation->active = 1;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_aes_transparent_cipher_decrypt_setup(
    ti_mspm33_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_status_t status;

    ti_mspm33_aesadv_init_hw();

    if (operation->active != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (alg != PSA_ALG_ECB_NO_PADDING && alg != PSA_ALG_CBC_NO_PADDING &&
        alg != PSA_ALG_CTR && alg != PSA_ALG_CFB && alg != PSA_ALG_OFB) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (key_buffer_size > TI_MSPM33_AES_MAX_KEY_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    operation->aesadv = AESADV;
    operation->alg = alg;
    operation->direction = 1;
    operation->key_size = key_buffer_size;
    memcpy(operation->key_material, key_buffer, key_buffer_size);
    operation->buffer_length = 0;
    operation->iv_set = 0;
    operation->active = 1;

    return PSA_SUCCESS;
}

/* ============================================================================
 * Cipher Multipart: Set IV
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_cipher_set_iv(
    ti_mspm33_cipher_operation_t *operation,
    const uint8_t *iv,
    size_t iv_length)
{
    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    if (operation->alg == PSA_ALG_ECB_NO_PADDING) {
        return PSA_ERROR_BAD_STATE;
    }

    if (iv_length != TI_MSPM33_AES_BLOCK_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memcpy(operation->iv, iv, iv_length);
    operation->iv_set = 1;

    return PSA_SUCCESS;
}

/* ============================================================================
 * Cipher Multipart: Update
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_cipher_update(
    ti_mspm33_cipher_operation_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    DL_AESADV_Config config = {0};
    DL_AESADV_KEY_SIZE key_size;
    size_t bytes_to_output = 0;
    size_t combined_length;
    size_t offset;

    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    if (operation->alg != PSA_ALG_ECB_NO_PADDING && !operation->iv_set) {
        return PSA_ERROR_BAD_STATE;
    }

    if (input_length == 0) {
        *output_length = 0;
        return PSA_SUCCESS;
    }

    combined_length = operation->buffer_length + input_length;
    bytes_to_output = (combined_length / TI_MSPM33_AES_BLOCK_SIZE) * TI_MSPM33_AES_BLOCK_SIZE;

    if (output_size < bytes_to_output) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (operation->key_size == 16) {
        key_size = DL_AESADV_KEY_SIZE_128_BIT;
    } else if (operation->key_size == 32) {
        key_size = DL_AESADV_KEY_SIZE_256_BIT;
    } else {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    DL_AESADV_setKey(AESADV, operation->key_material, key_size);

    if (operation->alg == PSA_ALG_ECB_NO_PADDING) {
        config.mode = DL_AESADV_MODE_ECB;
        config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
        config.lowerCryptoLength = 0;
        config.upperCryptoLength = 0;
        DL_AESADV_initECB(AESADV, &config);
    } else if (operation->alg == PSA_ALG_CBC_NO_PADDING) {
        config.mode = DL_AESADV_MODE_CBC;
        config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
        config.iv = operation->iv;
        config.lowerCryptoLength = 0;
        config.upperCryptoLength = 0;
        DL_AESADV_initCBC(AESADV, &config);
    } else if (operation->alg == PSA_ALG_CTR) {
        config.mode = DL_AESADV_MODE_CTR;
        config.direction = DL_AESADV_DIR_ENCRYPT;
        config.ctr_ctrWidth = DL_AESADV_CTR_WIDTH_128_BIT;
        config.iv = operation->iv;
        config.lowerCryptoLength = 0;
        config.upperCryptoLength = 0;
        DL_AESADV_initCTR(AESADV, &config);
    } else if (operation->alg == PSA_ALG_CFB) {
        config.mode = DL_AESADV_MODE_CFB;
        config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
        config.cfb_fbWidth = DL_AESADV_FB_WIDTH_128;
        config.iv = operation->iv;
        config.lowerCryptoLength = 0;
        config.upperCryptoLength = 0;
        DL_AESADV_initCFB(AESADV, &config);
    } else if (operation->alg == PSA_ALG_OFB) {
        config.mode = DL_AESADV_MODE_OFB;
        config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
        config.iv = operation->iv;
        config.lowerCryptoLength = 0;
        config.upperCryptoLength = 0;
        DL_AESADV_initOFB(AESADV, &config);
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Process buffered data + new input */
    uint8_t block[TI_MSPM33_AES_BLOCK_SIZE];
    offset = 0;

    while (combined_length >= TI_MSPM33_AES_BLOCK_SIZE) {
        size_t bytes_from_buffer = TI_MSPM33_AES_BLOCK_SIZE - operation->buffer_length;

        if (operation->buffer_length > 0) {
            memcpy(block, operation->buffer, operation->buffer_length);
            memcpy(block + operation->buffer_length, input, bytes_from_buffer);
            input += bytes_from_buffer;
            input_length -= bytes_from_buffer;
            operation->buffer_length = 0;
        } else {
            memcpy(block, input, TI_MSPM33_AES_BLOCK_SIZE);
            input += TI_MSPM33_AES_BLOCK_SIZE;
            input_length -= TI_MSPM33_AES_BLOCK_SIZE;
        }

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output + offset);

        offset += TI_MSPM33_AES_BLOCK_SIZE;
        combined_length -= TI_MSPM33_AES_BLOCK_SIZE;
    }

    /* Capture running IV/counter so next update() continues from correct state */
    if (bytes_to_output > 0 && operation->alg != PSA_ALG_ECB_NO_PADDING) {
        DL_AESADV_readIV(AESADV, operation->iv);
    }

    /* Buffer remaining input */
    if (input_length > 0) {
        memcpy(operation->buffer + operation->buffer_length, input, input_length);
        operation->buffer_length += input_length;
    }

    *output_length = bytes_to_output;
    return PSA_SUCCESS;
}

/* ============================================================================
 * Cipher Multipart: Finish
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_cipher_finish(
    ti_mspm33_cipher_operation_t *operation,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    if (operation->alg == PSA_ALG_ECB_NO_PADDING || operation->alg == PSA_ALG_CBC_NO_PADDING) {
        if (operation->buffer_length != 0) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        *output_length = 0;
    } else if (operation->alg == PSA_ALG_CTR ||
               operation->alg == PSA_ALG_CFB ||
               operation->alg == PSA_ALG_OFB) {
        if (operation->buffer_length > 0) {
            DL_AESADV_KEY_SIZE key_size;
            if (operation->key_size == 16) {
                key_size = DL_AESADV_KEY_SIZE_128_BIT;
            } else {
                key_size = DL_AESADV_KEY_SIZE_256_BIT;
            }

            DL_AESADV_Config config = {0};
            uint8_t block_buf[TI_MSPM33_AES_BLOCK_SIZE];

            DL_AESADV_setKey(AESADV, operation->key_material, key_size);
            if (operation->alg == PSA_ALG_CTR) {
                config.mode = DL_AESADV_MODE_CTR;
                config.direction = DL_AESADV_DIR_ENCRYPT;
                config.ctr_ctrWidth = DL_AESADV_CTR_WIDTH_128_BIT;
                config.iv = operation->iv;
                config.lowerCryptoLength = 0;
                config.upperCryptoLength = 0;
                DL_AESADV_initCTR(AESADV, &config);
            } else if (operation->alg == PSA_ALG_CFB) {
                config.mode = DL_AESADV_MODE_CFB;
                config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
                config.cfb_fbWidth = DL_AESADV_FB_WIDTH_128;
                config.iv = operation->iv;
                config.lowerCryptoLength = 0;
                config.upperCryptoLength = 0;
                DL_AESADV_initCFB(AESADV, &config);
            } else {
                config.mode = DL_AESADV_MODE_OFB;
                config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
                config.iv = operation->iv;
                config.lowerCryptoLength = 0;
                config.upperCryptoLength = 0;
                DL_AESADV_initOFB(AESADV, &config);
            }

            memset(block_buf, 0, TI_MSPM33_AES_BLOCK_SIZE);
            memcpy(block_buf, operation->buffer, operation->buffer_length);

            DL_AESADV_loadInputData(AESADV, block_buf);
            while (!DL_AESADV_isOutputReady(AESADV)) {
                __NOP();
            }
            DL_AESADV_readOutputData(AESADV, block_buf);

            if (output_size < operation->buffer_length) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            memcpy(output, block_buf, operation->buffer_length);
            *output_length = operation->buffer_length;
            operation->buffer_length = 0;
        } else {
            *output_length = 0;
        }
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    operation->active = 0;
    return PSA_SUCCESS;
}

/* ============================================================================
 * Cipher Multipart: Abort
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_cipher_abort(
    ti_mspm33_cipher_operation_t *operation)
{
    if (operation->active == 0) {
        return PSA_SUCCESS;
    }

    memset(operation->key_material, 0, TI_MSPM33_AES_MAX_KEY_SIZE);
    operation->key_size = 0;
    memset(operation->buffer, 0, TI_MSPM33_AES_BLOCK_SIZE);
    operation->buffer_length = 0;
    memset(operation->iv, 0, TI_MSPM33_AES_BLOCK_SIZE);
    operation->iv_set = 0;

    operation->active = 0;

    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Single-Shot: GCM Encrypt
 * ============================================================================ */

static psa_status_t ti_mspm33_aead_gcm_encrypt(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *nonce,
    size_t nonce_length,
    const uint8_t *additional_data,
    size_t additional_data_length,
    const uint8_t *plaintext,
    size_t plaintext_length,
    uint8_t *ciphertext,
    size_t ciphertext_size,
    size_t *ciphertext_length,
    DL_AESADV_KEY_SIZE key_size,
    size_t tag_length)
{
    DL_AESADV_Config config = {0};
    size_t offset;
    uint8_t tag[16];

    (void)key_buffer_size;

    if (ciphertext_size < plaintext_length + tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (nonce_length != 12) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (tag_length > 16 || tag_length < 4) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /*
     * Build the 16-byte GCM initial counter block J0 per NIST SP 800-38D §7.1:
     *   J0 = nonce (12 bytes) || 0x00000000 || 0x01
     * The counter field (bytes 12-15) must be exactly 0x00000001 (big-endian).
     * The buffer must be 4-byte aligned for DL_AESADV_loadIV.
     */
    uint8_t iv_buf[16] __attribute__((aligned(4)));
    memset(iv_buf, 0, sizeof(iv_buf));
    memcpy(iv_buf, nonce, 12);
    iv_buf[15] = 1; /* counter = 0x00000001 in big-endian */

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_GCM_AUTONOMOUS;
    config.direction = DL_AESADV_DIR_ENCRYPT;
    config.iv = iv_buf;
    config.aadLength = additional_data_length;
    config.lowerCryptoLength = plaintext_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initGCM(AESADV, &config);

    /* Feed AAD — poll INPUT_READY since AAD performs GHASH internally and produces no output */
    for (offset = 0; offset < additional_data_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (additional_data_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (additional_data_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, additional_data + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isInputReady(AESADV)) {
            __NOP();
        }
    }

    /* Feed plaintext and get ciphertext */
    for (offset = 0; offset < plaintext_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (plaintext_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (plaintext_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        uint8_t output_block[TI_MSPM33_AES_BLOCK_SIZE];
        memcpy(block, plaintext + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output_block);
        memcpy(ciphertext + offset, output_block, chunk_size);
    }

    /* Read tag */
    DL_AESADV_readTAG(AESADV, tag);
    memcpy(ciphertext + plaintext_length, tag, tag_length);

    *ciphertext_length = plaintext_length + tag_length;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Single-Shot: GCM Decrypt
 * ============================================================================ */

static psa_status_t ti_mspm33_aead_gcm_decrypt(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *nonce,
    size_t nonce_length,
    const uint8_t *additional_data,
    size_t additional_data_length,
    const uint8_t *ciphertext,
    size_t ciphertext_length,
    uint8_t *plaintext,
    size_t plaintext_size,
    size_t *plaintext_length,
    DL_AESADV_KEY_SIZE key_size,
    size_t tag_length)
{
    DL_AESADV_Config config = {0};
    size_t offset;
    uint8_t computed_tag[16];
    uint8_t input_tag[16];
    size_t data_length;

    (void)key_buffer_size;

    if (ciphertext_length < tag_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    data_length = ciphertext_length - tag_length;

    if (plaintext_size < data_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (nonce_length != 12) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memcpy(input_tag, ciphertext + data_length, tag_length);

    /*
     * Build the 16-byte GCM initial counter block J0 per NIST SP 800-38D §7.1:
     *   J0 = nonce (12 bytes) || 0x00000000 || 0x01
     * The counter field (bytes 12-15) must be exactly 0x00000001 (big-endian).
     * The buffer must be 4-byte aligned for DL_AESADV_loadIV.
     */
    uint8_t iv_buf[16] __attribute__((aligned(4)));
    memset(iv_buf, 0, sizeof(iv_buf));
    memcpy(iv_buf, nonce, 12);
    iv_buf[15] = 1; /* counter = 0x00000001 in big-endian */

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_GCM_AUTONOMOUS;
    config.direction = DL_AESADV_DIR_DECRYPT;
    config.iv = iv_buf;
    config.aadLength = additional_data_length;
    config.lowerCryptoLength = data_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initGCM(AESADV, &config);

    /* Feed AAD — poll INPUT_READY since AAD performs GHASH internally and produces no output */
    for (offset = 0; offset < additional_data_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (additional_data_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (additional_data_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, additional_data + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isInputReady(AESADV)) {
            __NOP();
        }
    }

    /* Feed ciphertext and get plaintext */
    for (offset = 0; offset < data_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (data_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (data_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        uint8_t output_block[TI_MSPM33_AES_BLOCK_SIZE];
        memcpy(block, ciphertext + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output_block);
        memcpy(plaintext + offset, output_block, chunk_size);
    }

    /* Read computed tag */
    DL_AESADV_readTAG(AESADV, computed_tag);

    /* Constant-time tag comparison */
    if (mbedtls_ct_memcmp(input_tag, computed_tag, tag_length) != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    *plaintext_length = data_length;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Single-Shot: CCM Encrypt
 * ============================================================================ */

static psa_status_t ti_mspm33_aead_ccm_encrypt(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *nonce,
    size_t nonce_length,
    const uint8_t *additional_data,
    size_t additional_data_length,
    const uint8_t *plaintext,
    size_t plaintext_length,
    uint8_t *ciphertext,
    size_t ciphertext_size,
    size_t *ciphertext_length,
    DL_AESADV_KEY_SIZE key_size,
    size_t tag_length)
{
    DL_AESADV_Config config = {0};
    DL_AESADV_CCM_CTR_WIDTH ctr_width;
    DL_AESADV_CCM_TAG_WIDTH tag_width;
    size_t offset;
    uint8_t tag[16];
    psa_status_t status;

    (void)key_buffer_size;

    if (ciphertext_size < plaintext_length + tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (nonce_length < 7 || nonce_length > 13) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (tag_length < 4 || tag_length > 8) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = _ti_mspm33_map_ccm_nonce_to_ctr_width(nonce_length, &ctr_width);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = _ti_mspm33_map_tag_length_to_ccm_tag_width(tag_length, &tag_width);
    if (status != PSA_SUCCESS) {
        return status;
    }

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_CCM;
    config.direction = DL_AESADV_DIR_ENCRYPT;
    config.ccm_ctrWidth = ctr_width;
    config.ccm_tagWidth = tag_width;
    /*
     * CCM uses config.nonce (not config.iv): DL_AESADV_initCCM() calls
     * DL_AESADV_loadCCMNonceAndCounter() which reads config->nonce to build
     * the CCM-specific A0 counter block.
     */
    config.nonce = (uint8_t *)nonce;
    config.aadLength = additional_data_length;
    config.lowerCryptoLength = plaintext_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initCCM(AESADV, &config);

    /* Feed AAD — poll INPUT_READY since AAD performs CBC-MAC internally and produces no output */
    for (offset = 0; offset < additional_data_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (additional_data_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (additional_data_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, additional_data + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isInputReady(AESADV)) {
            __NOP();
        }
    }

    /* Feed plaintext and get ciphertext */
    for (offset = 0; offset < plaintext_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (plaintext_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (plaintext_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, plaintext + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, ciphertext + offset);
    }

    /* Read tag */
    DL_AESADV_readTAG(AESADV, tag);
    memcpy(ciphertext + plaintext_length, tag, tag_length);

    *ciphertext_length = plaintext_length + tag_length;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Single-Shot: CCM Decrypt
 * ============================================================================ */

static psa_status_t ti_mspm33_aead_ccm_decrypt(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    const uint8_t *nonce,
    size_t nonce_length,
    const uint8_t *additional_data,
    size_t additional_data_length,
    const uint8_t *ciphertext,
    size_t ciphertext_length,
    uint8_t *plaintext,
    size_t plaintext_size,
    size_t *plaintext_length,
    DL_AESADV_KEY_SIZE key_size,
    size_t tag_length)
{
    DL_AESADV_Config config = {0};
    DL_AESADV_CCM_CTR_WIDTH ctr_width;
    DL_AESADV_CCM_TAG_WIDTH tag_width;
    size_t offset;
    size_t data_length;
    uint8_t computed_tag[16];
    uint8_t input_tag[16];
    psa_status_t status;

    (void)key_buffer_size;

    if (ciphertext_length < tag_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    data_length = ciphertext_length - tag_length;

    if (plaintext_size < data_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (nonce_length < 7 || nonce_length > 13) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (tag_length < 4 || tag_length > 8) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = _ti_mspm33_map_ccm_nonce_to_ctr_width(nonce_length, &ctr_width);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = _ti_mspm33_map_tag_length_to_ccm_tag_width(tag_length, &tag_width);
    if (status != PSA_SUCCESS) {
        return status;
    }

    memcpy(input_tag, ciphertext + data_length, tag_length);

    DL_AESADV_setKey(AESADV, key_buffer, key_size);

    config.mode = DL_AESADV_MODE_CCM;
    config.direction = DL_AESADV_DIR_DECRYPT;
    config.ccm_ctrWidth = ctr_width;
    config.ccm_tagWidth = tag_width;
    /*
     * CCM uses config.nonce (not config.iv): DL_AESADV_initCCM() calls
     * DL_AESADV_loadCCMNonceAndCounter() which reads config->nonce to build
     * the CCM-specific A0 counter block.
     */
    config.nonce = (uint8_t *)nonce;
    config.aadLength = additional_data_length;
    config.lowerCryptoLength = data_length;
    config.upperCryptoLength = 0;

    DL_AESADV_initCCM(AESADV, &config);

    /* Feed AAD — poll INPUT_READY since AAD performs CBC-MAC internally and produces no output */
    for (offset = 0; offset < additional_data_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (additional_data_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (additional_data_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, additional_data + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isInputReady(AESADV)) {
            __NOP();
        }
    }

    /* Feed ciphertext and get plaintext */
    for (offset = 0; offset < data_length; offset += TI_MSPM33_AES_BLOCK_SIZE) {
        size_t chunk_size = (data_length - offset > TI_MSPM33_AES_BLOCK_SIZE) ?
                            TI_MSPM33_AES_BLOCK_SIZE : (data_length - offset);
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, ciphertext + offset, chunk_size);

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, plaintext + offset);
    }

    /* Read computed tag */
    DL_AESADV_readTAG(AESADV, computed_tag);

    /* Constant-time tag comparison */
    if (mbedtls_ct_memcmp(input_tag, computed_tag, tag_length) != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    *plaintext_length = data_length;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Single-Shot Entry Points
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
    size_t *ciphertext_length)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_algorithm_t base_alg;
    size_t tag_length;
    psa_status_t status;

    ti_mspm33_aesadv_init_hw();

    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = _ti_mspm33_get_aead_tag_length(alg, &tag_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);

    if (base_alg == PSA_ALG_GCM) {
        return ti_mspm33_aead_gcm_encrypt(
            key_buffer, key_buffer_size, nonce, nonce_length,
            additional_data, additional_data_length,
            plaintext, plaintext_length,
            ciphertext, ciphertext_size, ciphertext_length,
            key_size, tag_length);
    } else if (base_alg == PSA_ALG_CCM) {
        return ti_mspm33_aead_ccm_encrypt(
            key_buffer, key_buffer_size, nonce, nonce_length,
            additional_data, additional_data_length,
            plaintext, plaintext_length,
            ciphertext, ciphertext_size, ciphertext_length,
            key_size, tag_length);
    }

    return PSA_ERROR_NOT_SUPPORTED;
}

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
    size_t *plaintext_length)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_algorithm_t base_alg;
    size_t tag_length;
    psa_status_t status;

    ti_mspm33_aesadv_init_hw();

    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = _ti_mspm33_get_aead_tag_length(alg, &tag_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);

    if (base_alg == PSA_ALG_GCM) {
        return ti_mspm33_aead_gcm_decrypt(
            key_buffer, key_buffer_size, nonce, nonce_length,
            additional_data, additional_data_length,
            ciphertext, ciphertext_length,
            plaintext, plaintext_size, plaintext_length,
            key_size, tag_length);
    } else if (base_alg == PSA_ALG_CCM) {
        return ti_mspm33_aead_ccm_decrypt(
            key_buffer, key_buffer_size, nonce, nonce_length,
            additional_data, additional_data_length,
            ciphertext, ciphertext_length,
            plaintext, plaintext_size, plaintext_length,
            key_size, tag_length);
    }

    return PSA_ERROR_NOT_SUPPORTED;
}

/* ============================================================================
 * AEAD Multipart: Setup
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_encrypt_setup(
    ti_mspm33_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_status_t status;

    if (operation->active != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    psa_algorithm_t base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);
    if (base_alg != PSA_ALG_GCM && base_alg != PSA_ALG_CCM) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (key_buffer_size > TI_MSPM33_AES_MAX_KEY_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ti_mspm33_aesadv_init_hw();

    operation->aesadv = AESADV;
    operation->alg = alg;
    operation->direction = 0;
    operation->key_size = key_buffer_size;
    memcpy(operation->key_material, key_buffer, key_buffer_size);
    operation->nonce_set = 0;
    operation->lengths_set = 0;
    operation->aad_buffer_length = 0;
    operation->pt_buffer_length = 0;
    operation->active = 1;

    return PSA_SUCCESS;
}

psa_status_t ti_mspm33_aes_transparent_aead_decrypt_setup(
    ti_mspm33_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    DL_AESADV_KEY_SIZE key_size;
    psa_status_t status;

    if (operation->active != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    status = _ti_mspm33_map_key_size(attributes, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    psa_algorithm_t base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);
    if (base_alg != PSA_ALG_GCM && base_alg != PSA_ALG_CCM) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (key_buffer_size > TI_MSPM33_AES_MAX_KEY_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ti_mspm33_aesadv_init_hw();

    operation->aesadv = AESADV;
    operation->alg = alg;
    operation->direction = 1;
    operation->key_size = key_buffer_size;
    memcpy(operation->key_material, key_buffer, key_buffer_size);
    operation->nonce_set = 0;
    operation->lengths_set = 0;
    operation->aad_buffer_length = 0;
    operation->pt_buffer_length = 0;
    operation->active = 1;

    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Multipart: Internal HW Configuration Helper
 * ============================================================================ */

static psa_status_t ti_mspm33_aead_configure_hw(ti_mspm33_aead_operation_t *operation)
{
    DL_AESADV_KEY_SIZE key_size;
    DL_AESADV_Config config = {0};
    psa_algorithm_t base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(operation->alg);

    if (operation->key_size == 16) {
        key_size = DL_AESADV_KEY_SIZE_128_BIT;
    } else if (operation->key_size == 32) {
        key_size = DL_AESADV_KEY_SIZE_256_BIT;
    } else {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    DL_AESADV_setKey(AESADV, operation->key_material, key_size);

    if (base_alg == PSA_ALG_GCM) {
        /*
         * Build the 16-byte GCM initial counter block J0 per NIST SP 800-38D §7.1:
         *   J0 = nonce (12 bytes) || 0x00000000 || 0x01
         * The counter field (bytes 12-15) must be exactly 0x00000001 (big-endian).
         * The buffer must be 4-byte aligned for DL_AESADV_loadIV.
         */
        uint8_t iv_buf[16] __attribute__((aligned(4)));
        memset(iv_buf, 0, sizeof(iv_buf));
        memcpy(iv_buf, operation->nonce, 12);
        iv_buf[15] = 1; /* counter = 0x00000001 in big-endian */

        config.mode = DL_AESADV_MODE_GCM_AUTONOMOUS;
        config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
        config.iv = iv_buf;
        config.aadLength = operation->lengths_set ? operation->aad_length : 0;
        config.lowerCryptoLength = operation->lengths_set ? operation->plaintext_length : 0;
        config.upperCryptoLength = 0;
        DL_AESADV_initGCM(AESADV, &config);
    } else if (base_alg == PSA_ALG_CCM) {
        DL_AESADV_CCM_CTR_WIDTH ctr_width;
        DL_AESADV_CCM_TAG_WIDTH tag_width;
        size_t tag_length;
        psa_status_t status;

        status = _ti_mspm33_get_aead_tag_length(operation->alg, &tag_length);
        if (status != PSA_SUCCESS) {
            return status;
        }
        status = _ti_mspm33_map_ccm_nonce_to_ctr_width(operation->nonce_length, &ctr_width);
        if (status != PSA_SUCCESS) {
            return status;
        }
        status = _ti_mspm33_map_tag_length_to_ccm_tag_width(tag_length, &tag_width);
        if (status != PSA_SUCCESS) {
            return status;
        }

        config.mode = DL_AESADV_MODE_CCM;
        config.direction = (operation->direction == 0) ? DL_AESADV_DIR_ENCRYPT : DL_AESADV_DIR_DECRYPT;
        config.ccm_ctrWidth = ctr_width;
        config.ccm_tagWidth = tag_width;
        /*
         * CCM uses config.nonce (not config.iv): DL_AESADV_initCCM() calls
         * DL_AESADV_loadCCMNonceAndCounter() which reads config->nonce to build
         * the CCM-specific A0 counter block.
         */
        config.nonce = operation->nonce;
        config.aadLength = operation->aad_length;
        config.lowerCryptoLength = operation->plaintext_length;
        config.upperCryptoLength = 0;
        DL_AESADV_initCCM(AESADV, &config);
    }

    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Multipart: Set Nonce
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_set_nonce(
    ti_mspm33_aead_operation_t *operation,
    const uint8_t *nonce,
    size_t nonce_length)
{
    psa_algorithm_t base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(operation->alg);

    if (operation->active != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    /* CCM requires set_lengths before set_nonce per PSA spec */
    if (base_alg == PSA_ALG_CCM && !operation->lengths_set) {
        return PSA_ERROR_BAD_STATE;
    }

    if (base_alg == PSA_ALG_GCM) {
        if (nonce_length != 12) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    } else if (base_alg == PSA_ALG_CCM) {
        if (nonce_length < 7 || nonce_length > 13) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    memcpy(operation->nonce, nonce, nonce_length);
    operation->nonce_length = nonce_length;
    operation->nonce_set = 1;

    return ti_mspm33_aead_configure_hw(operation);
}

/* ============================================================================
 * AEAD Multipart: Set Lengths
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_set_lengths(
    ti_mspm33_aead_operation_t *operation,
    size_t aad_length,
    size_t plaintext_length)
{
    /* Must be active and nonce must not yet be set — spec requires set_lengths before set_nonce */
    if (operation->active != 1 || operation->nonce_set) {
        return PSA_ERROR_BAD_STATE;
    }

    operation->aad_length = aad_length;
    operation->plaintext_length = plaintext_length;
    operation->lengths_set = 1;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Multipart: Update AD
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_update_ad(
    ti_mspm33_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    size_t combined;

    if (operation->active != 1 || !operation->nonce_set) {
        return PSA_ERROR_BAD_STATE;
    }

    combined = operation->aad_buffer_length + input_length;

    while (combined >= TI_MSPM33_AES_BLOCK_SIZE) {
        size_t bytes_from_buf = TI_MSPM33_AES_BLOCK_SIZE - operation->aad_buffer_length;
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE];

        if (operation->aad_buffer_length > 0) {
            memcpy(block, operation->aad_buffer, operation->aad_buffer_length);
            memcpy(block + operation->aad_buffer_length, input, bytes_from_buf);
            input += bytes_from_buf;
            input_length -= bytes_from_buf;
            operation->aad_buffer_length = 0;
        } else {
            memcpy(block, input, TI_MSPM33_AES_BLOCK_SIZE);
            input += TI_MSPM33_AES_BLOCK_SIZE;
            input_length -= TI_MSPM33_AES_BLOCK_SIZE;
        }

        DL_AESADV_loadInputData(AESADV, block);
        /* Poll INPUT_READY: AAD blocks produce no output, only update authentication state */
        while (!DL_AESADV_isInputReady(AESADV)) {
            __NOP();
        }

        combined -= TI_MSPM33_AES_BLOCK_SIZE;
    }

    if (input_length > 0) {
        memcpy(operation->aad_buffer + operation->aad_buffer_length, input, input_length);
        operation->aad_buffer_length += input_length;
    }

    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Multipart: Update (plaintext/ciphertext)
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_update(
    ti_mspm33_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    size_t combined;
    size_t bytes_to_output = 0;
    size_t offset = 0;

    if (operation->active != 1 || !operation->nonce_set) {
        return PSA_ERROR_BAD_STATE;
    }

    combined = operation->pt_buffer_length + input_length;
    bytes_to_output = (combined / TI_MSPM33_AES_BLOCK_SIZE) * TI_MSPM33_AES_BLOCK_SIZE;

    if (output_size < bytes_to_output) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    while (combined >= TI_MSPM33_AES_BLOCK_SIZE) {
        size_t bytes_from_buf = TI_MSPM33_AES_BLOCK_SIZE - operation->pt_buffer_length;
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE];

        if (operation->pt_buffer_length > 0) {
            memcpy(block, operation->pt_buffer, operation->pt_buffer_length);
            memcpy(block + operation->pt_buffer_length, input, bytes_from_buf);
            input += bytes_from_buf;
            input_length -= bytes_from_buf;
            operation->pt_buffer_length = 0;
        } else {
            memcpy(block, input, TI_MSPM33_AES_BLOCK_SIZE);
            input += TI_MSPM33_AES_BLOCK_SIZE;
            input_length -= TI_MSPM33_AES_BLOCK_SIZE;
        }

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, output + offset);

        offset += TI_MSPM33_AES_BLOCK_SIZE;
        combined -= TI_MSPM33_AES_BLOCK_SIZE;
    }

    if (input_length > 0) {
        memcpy(operation->pt_buffer + operation->pt_buffer_length, input, input_length);
        operation->pt_buffer_length += input_length;
    }

    *output_length = bytes_to_output;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Multipart: Finish (encrypt)
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_finish(
    ti_mspm33_aead_operation_t *operation,
    uint8_t *ciphertext_or_plaintext,
    size_t ciphertext_or_plaintext_size,
    size_t *ciphertext_or_plaintext_length,
    uint8_t *tag,
    size_t tag_size,
    size_t *tag_length)
{
    psa_status_t status;
    uint8_t hw_tag[16];

    if (operation->active != 1 || operation->direction != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    status = _ti_mspm33_get_aead_tag_length(operation->alg, tag_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (tag_size < *tag_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (operation->aad_buffer_length > 0) {
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, operation->aad_buffer, operation->aad_buffer_length);
        DL_AESADV_loadInputData(AESADV, block);
        /* Poll INPUT_READY: final partial AAD block produces no output */
        while (!DL_AESADV_isInputReady(AESADV)) {
            __NOP();
        }
        operation->aad_buffer_length = 0;
    }

    size_t offset = 0;
    if (operation->pt_buffer_length > 0) {
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, operation->pt_buffer, operation->pt_buffer_length);

        if (ciphertext_or_plaintext_size < operation->pt_buffer_length) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, ciphertext_or_plaintext);
        offset = operation->pt_buffer_length;
        operation->pt_buffer_length = 0;
    }

    DL_AESADV_readTAG(AESADV, hw_tag);
    memcpy(tag, hw_tag, *tag_length);
    *ciphertext_or_plaintext_length = offset;

    operation->active = 0;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Multipart: Verify (decrypt)
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_verify(
    ti_mspm33_aead_operation_t *operation,
    uint8_t *plaintext,
    size_t plaintext_size,
    size_t *plaintext_length,
    const uint8_t *tag,
    size_t tag_length)
{
    psa_status_t status;
    uint8_t computed_tag[16];
    size_t expected_tag_length;

    if (operation->active != 1 || operation->direction != 1) {
        return PSA_ERROR_BAD_STATE;
    }

    status = _ti_mspm33_get_aead_tag_length(operation->alg, &expected_tag_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (tag_length != expected_tag_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (operation->aad_buffer_length > 0) {
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, operation->aad_buffer, operation->aad_buffer_length);
        DL_AESADV_loadInputData(AESADV, block);
        /* Poll INPUT_READY: final partial AAD block produces no output */
        while (!DL_AESADV_isInputReady(AESADV)) {
            __NOP();
        }
        operation->aad_buffer_length = 0;
    }

    size_t offset = 0;
    if (operation->pt_buffer_length > 0) {
        uint8_t block[TI_MSPM33_AES_BLOCK_SIZE] = {0};
        memcpy(block, operation->pt_buffer, operation->pt_buffer_length);

        if (plaintext_size < operation->pt_buffer_length) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        DL_AESADV_loadInputData(AESADV, block);
        while (!DL_AESADV_isOutputReady(AESADV)) {
            __NOP();
        }
        DL_AESADV_readOutputData(AESADV, plaintext);
        offset = operation->pt_buffer_length;
        operation->pt_buffer_length = 0;
    }

    DL_AESADV_readTAG(AESADV, computed_tag);

    if (mbedtls_ct_memcmp(tag, computed_tag, tag_length) != 0) {
        operation->active = 0;
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    *plaintext_length = offset;
    operation->active = 0;
    return PSA_SUCCESS;
}

/* ============================================================================
 * AEAD Multipart: Abort
 * ============================================================================ */

psa_status_t ti_mspm33_aes_transparent_aead_abort(
    ti_mspm33_aead_operation_t *operation)
{
    if (operation->active == 0) {
        return PSA_SUCCESS;
    }

    memset(operation->key_material, 0, TI_MSPM33_AES_MAX_KEY_SIZE);
    operation->key_size = 0;
    memset(operation->nonce, 0, 16);
    operation->nonce_length = 0;
    memset(operation->aad_buffer, 0, TI_MSPM33_AES_BLOCK_SIZE);
    operation->aad_buffer_length = 0;
    memset(operation->pt_buffer, 0, TI_MSPM33_AES_BLOCK_SIZE);
    operation->pt_buffer_length = 0;

    operation->active = 0;

    return PSA_SUCCESS;
}
