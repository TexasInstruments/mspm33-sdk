/*
 * Copyright (c) 2015-2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "msp-crypto/msp_crypto.h"
#include <stdint.h>
#include "mcuboot_config/mcuboot_logging.h"
#include "string.h"

#include "sha256sw.h"
#include "ti/crypto/include/msp-crypto-lib.h"

#define KEYLEN 32  // in bytes

/* SHA256 Objects */
SHA256SW_Object sha256SWObject;
SHA256SW_Handle sha256SWHandle = &sha256SWObject;
int_fast16_t sha2SWresult;
uint32_t finalDigest[8];


void MSPCrypto_sha256_init(void)
{
    sha2SWresult = SHA256SW_start(sha256SWHandle, SHA2SW_HASH_TYPE_256);
}

void MSPCrypto_sha256_drop(void)
{
    SHA256SW_cancel(sha256SWHandle);
    return;
}

int MSPCrypto_sha256_update(const void *data, uint32_t data_len)
{
    if (sha2SWresult == SHA2SW_STATUS_SUCCESS) {
        sha2SWresult = SHA256SW_addData(sha256SWHandle, data, data_len);
    }
    return sha2SWresult;
}

int MSPCrypto_sha256_finish(uint8_t *output)
{
    if (sha2SWresult == SHA2SW_STATUS_SUCCESS) {
        sha2SWresult = SHA256SW_finalize(sha256SWHandle, (uint32_t *) output);
    }
    return sha2SWresult;
}

void MSPCrypto_ecdsa_p256_init(void)
{
    /* No initialization required for Crypto_ECDSA_P256_verify */
}

void MSPCrypto_ecdsa_p256_drop(void)
{
    /* Nothing required to drop */
    return;
}

int MSPCrypto_ecdsa_p256_verify(
    const uint8_t *pk, const uint8_t *hash, const uint8_t *sig)
{
    crypto_ec_stat_t ret;

    /* Key comes in pre-processed from DER format with x coordinate, then y.
     * Signature comes as r, then s.
     * Crypto_ECDSA_P256_verify expects separate pointers for each component.
     */
    ret = Crypto_ECDSA_P256_verify(
        pk,              /* pk_x: first 32 bytes */
        pk + KEYLEN,     /* pk_y: next 32 bytes */
        hash,            /* message digest */
        sig,             /* sig_r: first 32 bytes */
        sig + KEYLEN     /* sig_s: next 32 bytes */
    );

    if (ret != CRYPTO_EC_STAT_ECDSA_VALID_SIGNATURE) {
        MCUBOOT_LOG_ERR("MSPCrypto_ecdsa_p256_verify: fail");
        return -1;
    }

    return 0;
}