/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2021 STMicroelectronics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/crypto.h"
#include "low_level_rng.h"

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
/* As of mbed TLS 3.6, there is no support in the Core for the random entry points,
 * so the integration happens through the definition of MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
 * as the function that mbed TLS uses to retrieve random numbers from an external
 * generator. Note that we don't rely on redefining the type
 * mbedtls_psa_external_random_context_t available to the PSA Crypto core to make
 * keep integration simple, as there is no real gain in doing that.
 */
psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    (void)context;

    if (output == NULL || output_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    RNG_GetBytes(output, output_size, output_length);
    if (*output_length != output_size)
    {
    return -1;
    }
    return 0;
}
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

