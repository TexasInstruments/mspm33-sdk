/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_EFI_SOFT_CRC_H__
#define __TFM_EFI_SOFT_CRC_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Updates a CRC32 calculation using ISO 3309 CRC-32.
 *
 * \param[in] old_crc Existing CRC value (0 for the first calculation).
 * \param[in] buf     Buffer of bytes to perform the calculation over.
 * \param[in] len     Length of \p buf in bytes.
 *
 * \return The calculated CRC32 value.
 */
uint32_t efi_soft_crc32_update(uint32_t old_crc, const uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_EFI_SOFT_CRC_H__ */
