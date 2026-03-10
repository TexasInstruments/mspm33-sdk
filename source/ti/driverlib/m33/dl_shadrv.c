/*
 * Copyright (c) 2025, Texas Instruments Incorporated
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

#include "dl_shadrv.h"

/* Static function declarations */
static void DL_SHAW_loadData(
    volatile uint32_t *destPtr, const uint32_t *srcPtr, uint32_t numWords);
static void DL_SHAW_readData(
    uint32_t *destPtr, volatile const uint32_t *srcPtr, uint32_t numWords);

DL_SHAW_STATUS DL_SHAW_init(SHAW_Regs *shaw)
{
    DL_SHAW_STATUS status = DL_SHAW_STATUS_FAILURE;

    /* Assert Reset to SHA IP */
    DL_SHAW_reset(shaw);
    
    if (DL_SHAW_isReset(shaw)) {
        /* Enable power to SHA Engine */
        DL_SHAW_enablePower(shaw);
        
        if (DL_SHAW_isPowerEnabled(shaw)) {
            status = DL_SHAW_STATUS_SUCCESS;
        }
    }

    return status;
}

DL_SHAW_STATUS DL_SHAW_setModeConfig(SHAW_Regs *shaw, 
    DL_SHAW_ALGORITHM algorithm, DL_SHAW_MODE mode)
{
    uint32_t operationModeMask;
    uint32_t algoModeMask;
    uint32_t processingModeMask;

    /* Mask : Operation and Processing mode */
    if ( mode == DL_SHAW_MODE_HMAC )
    {
        /* HMAC computation */
        operationModeMask  = DL_SHAW_MODE_IN_HASH_HMAC_MASK;
        processingModeMask = DL_SHAW_MODE_IN_MAC_KEY_MASK;
    }
    else
    {
        /* SHA computation */
        operationModeMask  = ( uint32_t ) 0U;
        processingModeMask = ( uint32_t ) 0U;
    }
    /* Mask : Algorithm */
    if ( algorithm == DL_SHAW_ALGORITHM_SHA_256 )
    {
        algoModeMask = DL_SHAW_MODE_IN_SHA_256_MASK;
    }
    else if ( algorithm == DL_SHAW_ALGORITHM_SHA_224 )
    {
        algoModeMask = DL_SHAW_MODE_IN_SHA_224_MASK;
    }
    else
    {
        algoModeMask = ( uint32_t ) 0U;
    }

    shaw->SHAW.EIP59_MODE_IN =
        ( DL_SHAW_MODE_IN_NEW_HASH_MASK | processingModeMask | operationModeMask | algoModeMask );

    return DL_SHAW_STATUS_SUCCESS;
}

DL_SHAW_STATUS DL_SHAW_setDataLength(SHAW_Regs *shaw, 
    uint32_t lengthInBytes, DL_SHAW_MODE mode)
{
    uint64_t totalLengthBits;
    uint32_t lenL;
    uint32_t lenH;

    /* Calculate total length including HMAC block if applicable */
    if(mode == DL_SHAW_MODE_HMAC){
        lengthInBytes += DL_SHAW_BLOCK_SIZE_BYTES;
    }
    totalLengthBits                  = ( uint64_t ) ( lengthInBytes ) * 8U;
    lenL                              = ( uint32_t ) ( totalLengthBits & ( uint32_t ) 0xFFFFFFFFU );
    lenH                              = ( uint32_t ) ( ( totalLengthBits >> 32U ) & ( uint32_t ) 0xFFFFFFFFU );
    /* Update Length_IN Register in EIP */
    shaw->SHAW.EIP59_LENGTH_IN_L = lenL;
    shaw->SHAW.EIP59_LENGTH_IN_H = lenH;

    return DL_SHAW_STATUS_SUCCESS;
}

DL_SHAW_STATUS DL_SHAW_writeDataBlock(SHAW_Regs *shaw, 
    uint32_t *data, uint32_t length)
{
    if (!DL_SHAW_isInputReady(shaw)) {
        return DL_SHAW_STATUS_FAILURE;
    }

    /* Load data block */
    DL_SHAW_loadData(
        &shaw->SHAW.EIP59_DATA_IN0_, 
        (uint32_t *) data, 
        length / sizeof(uint32_t));

    return DL_SHAW_STATUS_SUCCESS;
}

DL_SHAW_STATUS DL_SHAW_writePartialBlock(SHAW_Regs *shaw, 
    uint32_t *data, uint32_t numWords, uint32_t numBytes)
{
    
    volatile uint32_t *ptrDataIn = &(shaw->SHAW.EIP59_DATA_IN0_);
    volatile uint32_t lastWord = 0;
    uint32_t          *p         = data;
    uint8_t index = 0;
    uint8_t byteNum = 0U;

    /* Write complete words first */
    for (index = 0; index < numWords; index++) {
        ptrDataIn[index] = p[index];
    }

    /* Handle remaining bytes if any */
    if (numBytes != 0) {
        p = p + index;
        for ( byteNum = 0U; byteNum < numBytes; byteNum = byteNum + 1U )
        {
            ( ( uint8_t * ) ( &lastWord ) )[byteNum] = ( ( uint8_t * ) p )[byteNum];
        }
        ptrDataIn[index] = lastWord;
    }

    return DL_SHAW_STATUS_SUCCESS;
}

DL_SHAW_STATUS DL_SHAW_EIPconsumeLastData( SHAW_Regs *shaw, DL_SHAW_MSGTYPE msgType )
{
    uint32_t mask =
        ( ( SHAW_EIP59_IO_BUF_CTRL_STAT_DATA_IN_AV_MASK ) | ( SHAW_EIP59_IO_BUF_CTRL_STAT_PAD_MESSAGE_MASK ) );
    if ( DL_SHAW_MSGTYPE_DATA == msgType )
    {
        /* Consume LENGTH_IN only if EIP is processing last 'DATA' */
        mask = ( mask | SHAW_EIP59_IO_BUF_CTRL_STAT_LENGTH_IN_AV_MASK );
    }
    shaw->SHAW.EIP59_IO_BUF_CTRL_STAT |= ( mask );
    return DL_SHAW_STATUS_SUCCESS;
}

DL_SHAW_STATUS DL_SHAW_getBlockInfo(uint32_t *data, 
    uint32_t length, DL_SHAW_BlockInfo *blockInfo)
{
    blockInfo->data = data;
    blockInfo->numWords = length / sizeof(uint32_t);
    blockInfo->numBlocks = blockInfo->numWords / DL_SHAW_BLOCK_SIZE_WORDS;
    blockInfo->numExtraWords = blockInfo->numWords % DL_SHAW_BLOCK_SIZE_WORDS;
    blockInfo->numExtraBytes = length % sizeof(uint32_t);

    return DL_SHAW_STATUS_SUCCESS;
}

DL_SHAW_STATUS DL_SHAW_readDigest(SHAW_Regs *shaw, uint32_t *digest, uint32_t length)
{
    
    if (!DL_SHAW_isOutputReady(shaw)) {
        return DL_SHAW_STATUS_FAILURE;
    }

    /* Read digest */
    DL_SHAW_readData(
        (uint32_t *)digest,
        (volatile const uint32_t *)&shaw->SHAW.EIP59_DIGEST_A,
        length / sizeof(uint32_t));

    return DL_SHAW_STATUS_SUCCESS;
}

void DL_SHAW_releaseDigest(SHAW_Regs *shaw)
{
    /* Clear the output buffer full flag */
    shaw->SHAW.EIP59_IO_BUF_CTRL_STAT |= DL_SHAW_IO_BUF_CTRL_OUTBUF_FULL;
}

void DL_SHAW_close(SHAW_Regs *shaw)
{
    /* Disable all interrupts */
    DL_SHAW_disableOutputReadyInterrupt(shaw);
    DL_SHAW_disableDMATriggerInterrupt(shaw);

    /* Disable power to SHA module */
    DL_SHAW_disablePower(shaw);

    /* Add delay after power disable */
    __asm(" NOP ");
    __asm(" NOP ");
    __asm(" NOP ");
    __asm(" NOP ");
}

/* Static helper functions */
static void DL_SHAW_loadData(
    volatile uint32_t *destPtr, const uint32_t *srcPtr, uint32_t numWords)
{
    uint32_t i;
    for (i = 0; i < numWords; i++) {
        destPtr[i] = srcPtr[i];
    }
}

static void DL_SHAW_readData(
    uint32_t *destPtr, volatile const uint32_t *srcPtr, uint32_t numWords)
{
    uint32_t i;
    for (i = 0; i < numWords; i++) {
        destPtr[i] = srcPtr[i];
    }
}