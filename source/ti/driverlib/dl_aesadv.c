/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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

#include <ti/driverlib/dl_aesadv.h>
#include <stdint.h>

#if defined(__MSPM0_HAS_AESADV__) || defined(__MSP_HAS_AESADV__)

static const uint32_t *DL_AESADV_checkAlignmentAndReturnConstPtr(
    const uint8_t *ptr);

static uint32_t *DL_AESADV_checkAlignmentAndReturnPtr(uint8_t *ptr);

DL_AESADV_STATUS DL_AESADV_setKey(
    AESADV_Regs *aesadv, const uint8_t *key, DL_AESADV_KEY_SIZE keySize)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    const uint32_t *keyAligned;

    keyAligned = DL_AESADV_checkAlignmentAndReturnConstPtr(key);
    if (keyAligned != NULL) {
        DL_AESADV_setKeyAligned(aesadv, keyAligned, keySize);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_setKeyAligned(AESADV_Regs *aesadv, const uint32_t *keyAligned,
    DL_AESADV_KEY_SIZE keySize)
{
    switch (keySize) {
        case DL_AESADV_KEY_SIZE_128_BIT:
            DL_Common_updateReg(
                &aesadv->CTRL, (uint32_t) keySize, AESADV_CTRL_KEYSIZE_MASK);
            /* Load 128-bit key (4 words) */
            aesadv->KEY0 = keyAligned[0];
            aesadv->KEY1 = keyAligned[1];
            aesadv->KEY2 = keyAligned[2];
            aesadv->KEY3 = keyAligned[3];
            break;

        case DL_AESADV_KEY_SIZE_256_BIT:
            DL_Common_updateReg(
                &aesadv->CTRL, (uint32_t) keySize, AESADV_CTRL_KEYSIZE_MASK);
            /* Load 256-bit key (8 words) */
            aesadv->KEY0 = keyAligned[0];
            aesadv->KEY1 = keyAligned[1];
            aesadv->KEY2 = keyAligned[2];
            aesadv->KEY3 = keyAligned[3];
            aesadv->KEY4 = keyAligned[4];
            aesadv->KEY5 = keyAligned[5];
            aesadv->KEY6 = keyAligned[6];
            aesadv->KEY7 = keyAligned[7];
            break;

        default:
            /* Invalid key size - no action taken */
            break;
    }
}

DL_AESADV_STATUS DL_AESADV_setGCMHashKey(
    AESADV_Regs *aesadv, const uint8_t *hashKey)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    const uint32_t *hashKeyAligned;

    hashKeyAligned = DL_AESADV_checkAlignmentAndReturnConstPtr(hashKey);
    if (hashKeyAligned != NULL) {
        DL_AESADV_setGCMHashKeyAligned(aesadv, hashKeyAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_setGCMHashKeyAligned(
    AESADV_Regs *aesadv, const uint32_t *hashKeyAligned)
{
    aesadv->GHASH_H0 = hashKeyAligned[0];
    aesadv->GHASH_H1 = hashKeyAligned[1];
    aesadv->GHASH_H2 = hashKeyAligned[2];
    aesadv->GHASH_H3 = hashKeyAligned[3];
}

DL_AESADV_STATUS DL_AESADV_setSecondKey(
    AESADV_Regs *aesadv, const uint8_t *secondKey)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    const uint32_t *secondKeyAligned;

    secondKeyAligned = DL_AESADV_checkAlignmentAndReturnConstPtr(secondKey);
    if (secondKeyAligned != NULL) {
        DL_AESADV_setSecondKeyAligned(aesadv, secondKeyAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_setSecondKeyAligned(
    AESADV_Regs *aesadv, const uint32_t *secondKeyAligned)
{
    aesadv->GHASH_H0 = secondKeyAligned[0];
    aesadv->GHASH_H1 = secondKeyAligned[1];
    aesadv->GHASH_H2 = secondKeyAligned[2];
    aesadv->GHASH_H3 = secondKeyAligned[3];
}

DL_AESADV_STATUS DL_AESADV_setThirdKey(
    AESADV_Regs *aesadv, const uint8_t *thirdKey)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    const uint32_t *thirdKeyAligned;

    thirdKeyAligned = DL_AESADV_checkAlignmentAndReturnConstPtr(thirdKey);
    if (thirdKeyAligned != NULL) {
        DL_AESADV_setThirdKeyAligned(aesadv, thirdKeyAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_setThirdKeyAligned(
    AESADV_Regs *aesadv, const uint32_t *thirdKeyAligned)
{
    aesadv->GCMCCM_TAG0 = thirdKeyAligned[0];
    aesadv->GCMCCM_TAG1 = thirdKeyAligned[1];
    aesadv->GCMCCM_TAG2 = thirdKeyAligned[2];
    aesadv->GCMCCM_TAG3 = thirdKeyAligned[3];
}

DL_AESADV_STATUS DL_AESADV_loadIntermediateTAG(
    AESADV_Regs *aesadv, const uint8_t *tag)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    const uint32_t *tagAligned;

    tagAligned = DL_AESADV_checkAlignmentAndReturnConstPtr(tag);
    if (tagAligned != NULL) {
        DL_AESADV_loadIntermediateTAGAligned(aesadv, tagAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_loadIntermediateTAGAligned(
    AESADV_Regs *aesadv, const uint32_t *tagAligned)
{
    aesadv->GCMCCM_TAG0 = tagAligned[0];
    aesadv->GCMCCM_TAG1 = tagAligned[1];
    aesadv->GCMCCM_TAG2 = tagAligned[2];
    aesadv->GCMCCM_TAG3 = tagAligned[3];
}

DL_AESADV_STATUS DL_AESADV_loadIV(
    AESADV_Regs *aesadv, const uint8_t *iv)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    const uint32_t *ivAligned;

    ivAligned = DL_AESADV_checkAlignmentAndReturnConstPtr(iv);
    if (ivAligned != NULL) {
        DL_AESADV_loadIVAligned(aesadv, ivAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_loadIVAligned(
    AESADV_Regs *aesadv, const uint32_t *ivAligned)
{
    aesadv->IV0 = ivAligned[0];
    aesadv->IV1 = ivAligned[1];
    aesadv->IV2 = ivAligned[2];
    aesadv->IV3 = ivAligned[3];
}

DL_AESADV_STATUS DL_AESADV_readIV(
    AESADV_Regs *aesadv, uint8_t *iv)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    uint32_t *ivAligned;

    ivAligned = DL_AESADV_checkAlignmentAndReturnPtr(iv);
    if (ivAligned != NULL) {
        DL_AESADV_readIVAligned(aesadv, ivAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_readIVAligned(
    const AESADV_Regs *aesadv, uint32_t *ivAligned)
{
    ivAligned[0] = aesadv->IV0;
    ivAligned[1] = aesadv->IV1;
    ivAligned[2] = aesadv->IV2;
    ivAligned[3] = aesadv->IV3;
}

void DL_AESADV_loadCCMNonceAndCounter(
    AESADV_Regs *aesadv, uint8_t *nonce, DL_AESADV_CCM_CTR_WIDTH ctrWidth)
{
    uint8_t *noncePtr = nonce;
    uint32_t ivArray[4];
    /* MISRA compliant multi-step cast */
    void *voidPtr = (void *) &ivArray[0];
    uint8_t *ivPtr = (uint8_t *) voidPtr;
    uint8_t i;

    /* calculation of the nonce length. (Length described in table in header) */
    uint8_t ccml = (uint8_t)((uint32_t) ctrWidth >> AESADV_CTRL_CCML_OFS);
    uint8_t counterWidthInBytes = (ccml + 1U);

    /* subtracting the counterWidth in bytes as well as the flag byte */
    uint8_t nonceWidthInBytes = 14U - ccml;

    *ivPtr = ccml;
    ivPtr++;

    /* addition of the Nonce */
    for (i = 0; i < nonceWidthInBytes; i++) {
        *ivPtr = *noncePtr;
        ivPtr++;
        noncePtr++;
    }

    /* Counter is always initialized to zeros */
    for (i = 0; i < counterWidthInBytes; i++) {
        *ivPtr = 0x00;
        ivPtr++;
    }

    aesadv->IV0 = ivArray[0];
    aesadv->IV1 = ivArray[1];
    aesadv->IV2 = ivArray[2];
    aesadv->IV3 = ivArray[3];
}

DL_AESADV_STATUS DL_AESADV_loadInputData(
    AESADV_Regs *aesadv, const uint8_t *data)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    const uint32_t *dataAligned;

    dataAligned = DL_AESADV_checkAlignmentAndReturnConstPtr(data);
    if (dataAligned != NULL) {
        DL_AESADV_loadInputDataAligned(aesadv, dataAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_loadInputDataAligned(
    AESADV_Regs *aesadv, const uint32_t *dataAligned)
{
    aesadv->DATA0 = dataAligned[0];
    aesadv->DATA1 = dataAligned[1];
    aesadv->DATA2 = dataAligned[2];
    aesadv->DATA3 = dataAligned[3];
}

DL_AESADV_STATUS DL_AESADV_readOutputData(
    const AESADV_Regs *aesadv, uint8_t *data)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    uint32_t *dataAligned;

    dataAligned = DL_AESADV_checkAlignmentAndReturnPtr(data);
    if (dataAligned != NULL) {
        DL_AESADV_readOutputDataAligned(aesadv, dataAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_readOutputDataAligned(
    const AESADV_Regs *aesadv, uint32_t *dataAligned)
{
    dataAligned[0] = aesadv->DATA0;
    dataAligned[1] = aesadv->DATA1;
    dataAligned[2] = aesadv->DATA2;
    dataAligned[3] = aesadv->DATA3;
}

DL_AESADV_STATUS DL_AESADV_readTAG(
    const AESADV_Regs *aesadv, uint8_t *tag)
{
    DL_AESADV_STATUS status = DL_AESADV_STATUS_SUCCESS;
    uint32_t *tagAligned;

    tagAligned = DL_AESADV_checkAlignmentAndReturnPtr(tag);
    if (tagAligned != NULL) {
        DL_AESADV_readTAGAligned(aesadv, tagAligned);
    } else {
        status = DL_AESADV_STATUS_UNALIGNED_ACCESS;
    }
    return status;
}

void DL_AESADV_readTAGAligned(const AESADV_Regs *aesadv, uint32_t *tagAligned)
{
    tagAligned[0] = aesadv->TAG0;
    tagAligned[1] = aesadv->TAG1;
    tagAligned[2] = aesadv->TAG2;
    tagAligned[3] = aesadv->TAG3;
}

void DL_AESADV_initECB(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t direction = (uint32_t)config->direction;

    DL_Common_updateReg(&aesadv->CTRL,
        ((uint32_t) DL_AESADV_MODE_ECB | direction),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initCBC(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t direction = (uint32_t)config->direction;

    DL_Common_updateReg(&aesadv->CTRL,
        ((uint32_t) DL_AESADV_MODE_CBC | direction),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    (void) DL_AESADV_loadIV(aesadv, config->iv);

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initCFB(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t direction = (uint32_t)config->direction;
    uint32_t fbWidth = (uint32_t)config->cfb_fbWidth;

    DL_Common_updateReg(&aesadv->CTRL,
        (((uint32_t) DL_AESADV_MODE_CFB) | direction | fbWidth),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_CTR_WIDTH_MASK | (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    (void) DL_AESADV_loadIV(aesadv, config->iv);

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initOFB(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t direction = (uint32_t)config->direction;
    uint32_t fbWidth = (uint32_t)config->cfb_fbWidth;

    DL_Common_updateReg(&aesadv->CTRL,
        (((uint32_t) DL_AESADV_MODE_OFB) | direction | fbWidth),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_CTR_WIDTH_MASK | (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    (void) DL_AESADV_loadIV(aesadv, config->iv);

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initCTR(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t direction = (uint32_t)config->direction;
    uint32_t ctrWidth = (uint32_t)config->ctr_ctrWidth;

    DL_Common_updateReg(&aesadv->CTRL,
        (((uint32_t) DL_AESADV_MODE_CTR) | direction | ctrWidth),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_CTR_WIDTH_MASK | (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    (void) DL_AESADV_loadIV(aesadv, config->iv);

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initICM(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t direction = (uint32_t)config->direction;

    DL_Common_updateReg(&aesadv->CTRL,
        ((uint32_t) DL_AESADV_MODE_ICM | direction),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    (void) DL_AESADV_loadIV(aesadv, config->iv);

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initCMAC(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t zeroArray[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

    /* Set the second and third keys, and zero out the initialization vector */
    (void) DL_AESADV_setSecondKey(aesadv, config->k1);
    (void) DL_AESADV_setThirdKey(aesadv, config->k2);
    (void)DL_AESADV_loadIVAligned(aesadv, &zeroArray[0]);

    DL_Common_updateReg(&aesadv->CTRL,
        ((uint32_t) DL_AESADV_MODE_CMAC | ((uint32_t) DL_AESADV_DIR_ENCRYPT) |
            ((uint32_t) AESADV_CTRL_SAVE_CNTXT_ENABLE)),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initCBCMAC(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t zeroArray[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

    /* Zero out the second and third keys and the IV for CBC-MAC as per spec */
    DL_AESADV_setSecondKeyAligned(aesadv, &zeroArray[0]);
    DL_AESADV_setThirdKeyAligned(aesadv, &zeroArray[0]);
    DL_AESADV_loadIVAligned(aesadv, &zeroArray[0]);

    DL_Common_updateReg(&aesadv->CTRL,
        ((uint32_t) DL_AESADV_MODE_CBCMAC |
            ((uint32_t) DL_AESADV_DIR_ENCRYPT) |
            ((uint32_t) AESADV_CTRL_SAVE_CNTXT_ENABLE)),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);
}

void DL_AESADV_initGCM(AESADV_Regs *aesadv, const DL_AESADV_Config *config)
{
    uint32_t zeroArray[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
    uint32_t mode = (uint32_t)config->mode;
    uint32_t direction = (uint32_t)config->direction;

    /* Zero out the intermediate TAG for GCM as per spec */
    DL_AESADV_loadIntermediateTAGAligned(aesadv, &zeroArray[0]);

    (void) DL_AESADV_loadIV(aesadv, config->iv);

    DL_Common_updateReg(&aesadv->CTRL,
        (mode | direction |
            ((uint32_t) DL_AESADV_CTR_WIDTH_32_BIT |
                (uint32_t) AESADV_CTRL_SAVE_CNTXT_ENABLE)),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK |
            (uint32_t) AESADV_CTRL_CTR_WIDTH_MASK | (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);

    DL_AESADV_setAADLength(aesadv, config->aadLength);
}

void DL_AESADV_initCCM(AESADV_Regs *aesadv, DL_AESADV_Config *config)
{
    uint32_t direction = (uint32_t)config->direction;
    uint32_t ccmCtrWidth = (uint32_t)config->ccm_ctrWidth;
    uint32_t ccmTagWidth = (uint32_t)config->ccm_tagWidth;

    DL_AESADV_loadCCMNonceAndCounter(
        aesadv, config->nonce, config->ccm_ctrWidth);

    DL_Common_updateReg(&aesadv->CTRL,
        (((uint32_t) DL_AESADV_MODE_CCM) | direction |
            ccmCtrWidth | ccmTagWidth |
            ((uint32_t) DL_AESADV_CTR_WIDTH_64_BIT) |
            ((uint32_t) AESADV_CTRL_SAVE_CNTXT_ENABLE)),
        ((uint32_t) DL_AESADV_MODE_MASK | (uint32_t) AESADV_CTRL_DIR_MASK | (uint32_t) AESADV_CTRL_CCML_MASK |
            (uint32_t) AESADV_CTRL_CCMM_MASK | (uint32_t) AESADV_CTRL_CTR_WIDTH_MASK |
            (uint32_t) AESADV_CTRL_SAVE_CNTXT_MASK));

    DL_AESADV_setUpperCryptoLength(aesadv, config->upperCryptoLength);
    DL_AESADV_setLowerCryptoLength(aesadv, config->lowerCryptoLength);

    DL_AESADV_setAADLength(aesadv, config->aadLength);
}

static const uint32_t *DL_AESADV_checkAlignmentAndReturnConstPtr(
    const uint8_t *ptr)
{
    uintptr_t address = (uintptr_t) ptr;
    const uint32_t *alignedPtr;
    if ((address << 30) != 0x00000000u) {
        /* Unaligned access */
        alignedPtr = NULL;
    } else {
        /* Aligned Access - MISRA compliant multi-step cast */
        const void *voidPtr = (const void *) ptr;
        alignedPtr = (const uint32_t *) voidPtr;
    }
    return alignedPtr;
}

static uint32_t *DL_AESADV_checkAlignmentAndReturnPtr(uint8_t *ptr)
{
    uintptr_t address = (uintptr_t) ptr;
    uint32_t *alignedPtr;
    if ((address << 30) != 0x00000000u) {
        /* Unaligned access */
        alignedPtr = NULL;
    } else {
        /* Aligned Access - Multi-step cast */
        void *voidPtr = (void *) ptr;
        alignedPtr = (uint32_t *) voidPtr;
    }
    return alignedPtr;
}

#endif /* __MSPM0_HAS_AESADV__ || __MSP_HAS_AESADV__*/
