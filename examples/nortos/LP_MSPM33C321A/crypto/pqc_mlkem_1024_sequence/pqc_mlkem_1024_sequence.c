 /*
 * Copyright (c) 2026, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       pqc_mlkem_1024_sequence.c
 *  @brief      PQC module ML-KEM-1024 sequence example
 * 
 *  This code example demonstrates how to use the ML-KEM-1024 key encapsulation
 *  mechanism to perform a key generation, encapsulation, and decapsulation 
 *  sequence.
 * 
 *  The entropy source (TRNG) is used to seed a DRBG which is subsequently used
 *  to generate the random seed values for ML-KEM-1024.  From the seed values, 
 *  ML-KEM-1024 encapsulation and decapsulation keys are first generated.  
 *  Then, the encapsulation key is used with additional random seed data to 
 *  generate a ciphertext and shared secret.  Finally, the
 *  decapsulation function is used with the decapsulation key to take the 
 *  ciphertext and generate the same shared secret.
 * 
 *  In the end, the shared secret values are expected to match for the
 *  example to pass.
 * 
 *  This example combines encapsulation and decapsulation into a single code 
 *  example to show the sequence. In a real world application, ML-KEM-1024 
 *  would be used by two processors/systems to agree to a shared secret over 
 *  an unsecure channel, where on side performs encapsulation and the other 
 *  side performs decapsulation, and the ciphertext is shared across the 
 *  unsecure channel.
 *
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "ti/crypto/include/msp-crypto-lib.h"

/* The DRBG used to generate random values in this example */
crypto_drbg_ctx_t drbg;

/* Buffer to store the entropy seed for the DRBG */
uint32_t rand[12];

/* Buffer to store the random seed for key generation */
uint32_t keyGenSeed[16];

/* Buffer to store the random seed for encapsulation */
uint32_t encapSeed[8];

/* Buffer to store the encapsulation key */
uint8_t ek[CRYPTO_MLKEM_1024_EK_LEN];

/* Buffer to store the decapsulation key */
uint8_t dk[CRYPTO_MLKEM_1024_DK_LEN];

/* Buffer to store the encapsulation ciphertext */
uint8_t ct[CRYPTO_MLKEM_1024_CIPHER_TEXT_LEN];

/* Buffer to store the shared secret generated during encapsulation */
uint8_t ss_enc[CRYPTO_MLKEM_SHARED_SECRET_LEN];

/* Buffer to store the shared secret generated during decapsulation */
uint8_t ss_dec[CRYPTO_MLKEM_SHARED_SECRET_LEN];

int main(void)
{
    crypto_mlkem_stat_t stat;
    uint32_t i;

    SYSCFG_DL_init();

    /* First we will collect entropy from the TRNG enropy source and seed the
     * DRBG so that we can generate random values.
     */
    Crypto_getEntropy(rand, 12U, 32000000U);
    Crypto_DRBG_initialize(&drbg, &rand[0], &rand[8]);

    /* Next we will generate a random seed for use in ML-KEM-1024 
     * key generation. Then we will generate the ML-KEM-1024 encap/decap keys.
     */
    Crypto_DRBG_generate(&drbg, keyGenSeed, 16U);
    stat = Crypto_MLKEM_1024_keyGen(
        (uint8_t*)keyGenSeed, 
        sizeof(keyGenSeed), 
        ek, 
        dk);
    if (stat != CRYPTO_MLKEM_STAT_OK) {
        goto err;
    }

    /* Next we will generate the shared secret and ciphertext (encapsulation)
     * using the encap key and another random seed.
     */
    Crypto_DRBG_generate(&drbg, encapSeed, 8U);
    stat = Crypto_MLKEM_1024_encaps(
        (uint8_t*)encapSeed, 
        sizeof(encapSeed), 
        ek, 
        ct, 
        ss_enc);
    if (stat != CRYPTO_MLKEM_STAT_OK) {
        goto err;
    }

    /* Finally we will generate the shared secret using the ciphertext
     * and decap key.  This step would normally be performed on
     * another processor that received the ciphertext, but to keep
     * this example simple, it is performed here to that the full
     * ML-KEM sequence can be demonstrated on one device.
     */
    stat = Crypto_MLKEM_1024_decaps(
        dk, 
        ct, 
        ss_dec);
    if (stat != CRYPTO_MLKEM_STAT_OK) {
        goto err;
    }

    /* Check generated shared secret values for equality */
    for (i=0; i<CRYPTO_MLKEM_SHARED_SECRET_LEN; i++) {
        if (ss_enc[i] != ss_dec[i]) {
            goto err;
        }
    }

    /* Trap here on a pass condition */
    while (1) { 
        __NOP(); 
    }

err:
    /* Trap here on an error condition */
    while (1) { 
        __NOP(); 
    }
}
