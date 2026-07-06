/*
 * Copyright (c) 2024-2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cy_device.h"
#include "coverity_check.h"
#include "tfm_plat_crypto_nv_seed.h"
#include <string.h>

TFM_COVERITY_DEVIATE_BLOCK(MISRA_C_2023_Directive_4_6, "Writing functions as in ARM templates, so we keep numerical types")
int tfm_plat_crypto_provision_entropy_seed(void)
{
    /* This function is used in tfm_crypto_engine_init and should returns success value */
    return TFM_CRYPTO_NV_SEED_SUCCESS;
}

int tfm_plat_crypto_nv_seed_write(const unsigned char *buf, size_t buf_len)
{
    (void)buf;
    (void)buf_len;

    /* This function is used in mbedtls_entropy_update_nv_seed and should returns success value */
    return TFM_CRYPTO_NV_SEED_SUCCESS;
}
TFM_COVERITY_BLOCK_END(MISRA_C_2023_Directive_4_6)
