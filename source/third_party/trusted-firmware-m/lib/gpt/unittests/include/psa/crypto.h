/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/error.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Generate random bytes.
 *
 * \warning This function **can** fail! Callers MUST check the return status
 *          and MUST NOT use the content of the output buffer if the return
 *          status is not #PSA_SUCCESS.
 *
 * \note    To generate a key, use psa_generate_key() instead.
 *
 * \param[out] output       Output buffer for the generated data.
 * \param output_size       Number of bytes to generate and output.
 *
 * \retval #PSA_SUCCESS \emptydescription
 * \retval #PSA_ERROR_NOT_SUPPORTED \emptydescription
 * \retval #PSA_ERROR_INSUFFICIENT_ENTROPY \emptydescription
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_HARDWARE_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_generate_random(uint8_t *output,
                                 size_t output_size);

#ifdef __cplusplus
}
#endif