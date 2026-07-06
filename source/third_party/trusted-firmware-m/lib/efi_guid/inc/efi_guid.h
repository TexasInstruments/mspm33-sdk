/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_EFI_GUID_H__
#define __TFM_EFI_GUID_H__

#include "psa/crypto.h"
#include "efi_guid_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Generates a random version 4 UUID/GUID using a PSA crypto driver.
 *
 * \note See RFC 4112 for details on UUID/GUIDs.
 *
 * \note See psa_generate_random for possible failures.
 *
 * \param[out] guid Pointer populated with the generated UUID/GUID.
 *
 * \return PSA_SUCCESS on success or a PSA error code on failure.
 *
 */
psa_status_t efi_guid_generate_random(struct efi_guid_t *guid);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_EFI_GUID_H__ */
