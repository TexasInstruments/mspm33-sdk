/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

#include "psa/crypto.h"

#include "efi_guid_structs.h"
#include "efi_guid.h"

psa_status_t efi_guid_generate_random(struct efi_guid_t *guid)
{
    const psa_status_t ret = psa_generate_random((uint8_t *)guid, sizeof(*guid));
    if (ret != PSA_SUCCESS) {
        return ret;
    }

    /* According to RFC 4122, counting bits from the right:
     * - Bits 6 and 7 of clock_seq_hi_and_reserved need to be set to
     *   0 and 1 respecitively
     * - Bits 12 through 15 of time_hi_and_version need to be set to
     *   0b0100
     */
    guid->clock_seq_hi_and_reserved &= 0x3F;
    guid->clock_seq_hi_and_reserved |= 0x80;
    guid->time_hi_and_version &= 0x0FFF;
    guid->time_hi_and_version |= 0x4000;

    return PSA_SUCCESS;
}
