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

/*!****************************************************************************
 *  @file       dl_shadrv.h
 *  @brief      SHA Hardware Accelerator (SHAW) Driver Library
 *  @defgroup   SHAW Secure Hash Algorithm Hardware Accelerator (SHAW)
 *
 *  @anchor ti_dl_dl_shaw_Overview
 *  # Overview
 *
 *  The SHAW DriverLib allows full configuration of the MSPM0 SHAW module.
 *  The SHAW accelerator module provides hardware acceleration for SHA-224 
 *  and SHA-256 hash operations and HMAC authentication.
 *
 *  <hr>
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

/* clang-format off */

/* Constants */
#define DL_SHAW_BLOCK_SIZE_BYTES             (64U)  /* 512-bit block size */
#define DL_SHAW_DIGEST_SIZE_224_BYTES        (28U)  /* SHA-224 digest size */
#define DL_SHAW_DIGEST_SIZE_256_BYTES        (32U)  /* SHA-256 digest size */
#define DL_SHAW_MAX_DIGEST_SIZE_BYTES        (32U)  /* Maximum digest size */
#define DL_SHAW_WORD_SIZE_BYTES              (4U)   /* Size of a word in bytes */
#define DL_SHAW_BLOCK_SIZE_WORDS             (16U)  /* Block size in words */

/* Register bit definitions and masks */
#define DL_SHAW_MODE_IN_NEW_HASH_MASK        SHAW_EIP59_MODE_IN_NEW_HASH_MASK
#define DL_SHAW_MODE_IN_MAC_KEY_MASK         SHAW_EIP59_MODE_IN_MAC_KEY_MASK
#define DL_SHAW_MODE_IN_HASH_HMAC_MASK       SHAW_EIP59_MODE_IN_HASH_HMAC_MASK
#define DL_SHAW_MODE_IN_SHA_224_MASK         SHAW_EIP59_MODE_IN_SHA_224_MASK
#define DL_SHAW_MODE_IN_SHA_256_MASK         SHAW_EIP59_MODE_IN_SHA_256_MASK

#define DL_SHAW_IO_BUF_CTRL_DATA_IN_AV       SHAW_EIP59_IO_BUF_CTRL_STAT_DATA_IN_AV_MASK
#define DL_SHAW_IO_BUF_CTRL_LENGTH_IN_AV     SHAW_EIP59_IO_BUF_CTRL_STAT_LENGTH_IN_AV_MASK
#define DL_SHAW_IO_BUF_CTRL_MODE_IN_AV       SHAW_EIP59_IO_BUF_CTRL_STAT_MODE_IN_AV_MASK
#define DL_SHAW_IO_BUF_CTRL_OUTBUF_FULL      SHAW_EIP59_IO_BUF_CTRL_STAT_OUTBUF_FULL_MASK
#define DL_SHAW_IO_BUF_CTRL_PAD_MESSAGE      SHAW_EIP59_IO_BUF_CTRL_STAT_PAD_MESSAGE_MASK
#define DL_SHAW_IO_BUF_CTRL_GET_DIGEST       SHAW_EIP59_IO_BUF_CTRL_STAT_GET_DIGEST_MASK
#define DL_SHAW_DATA_IN_ADDR(INST)           ((uint32_t)&(INST)->SHAW.SHAW_DATA_FIXED)

/** @addtogroup DL_SHAW_INTERRUPTS
 *  @{
 */
/*!
 *  @brief SHAW Output Ready interrupt
 */
#define DL_SHAW_INTERRUPT_OUTPUT_READY        SHAW_SHAW_PUB0_IMASK_OUTPUTRDY_MASK

/*!
 *  @brief SHAW DMA Trigger interrupt  
 */
#define DL_SHAW_INTERRUPT_DMA_TRIGGER         SHAW_SHAW_PUB1_IMASK_DMA_START_TRIGGER_MASK
/** @}*/

/*! @enum DL_SHAW_IIDX */
typedef enum {
    /*! SHAW interrupt index for output ready */
    DL_SHAW_IIDX_OUTPUT_READY = SHAW_SHAW_PUB0_IIDX_STAT_MASK,
    /*! SHAW interrupt index for DMA trigger */
    DL_SHAW_IIDX_DMA_TRIGGER = SHAW_SHAW_PUB1_IIDX_STAT_MASK
} DL_SHAW_IIDX;

/*!
 * @brief SHAW status return codes
 */
typedef enum {
    DL_SHAW_STATUS_SUCCESS = 0x12345678U,
    DL_SHAW_STATUS_FAILURE = 0xABCD0123U
} DL_SHAW_STATUS;

/*!
 * @brief SHAW algorithm selection
 */
typedef enum {
    DL_SHAW_ALGORITHM_SHA_224 = 0x33F80A12U,
    DL_SHAW_ALGORITHM_SHA_256 = 0x59A23BC4U
} DL_SHAW_ALGORITHM;

/*!
 * @brief SHAW operation mode
 */
typedef enum {
    DL_SHAW_MODE_HASH = 0x9085F0BDU,
    DL_SHAW_MODE_HMAC = 0xA1533F08U
} DL_SHAW_MODE;

/*!
 * @brief SHAW message type
 */
typedef enum
{
    DL_SHAW_MSGTYPE_DATA = 0x2E59C008U,
    DL_SHAW_MSGTYPE_KEY  = 0x5A110FD9U
} DL_SHAW_MSGTYPE;

/*!
 * @brief SHAW returne type
 */
typedef enum
{
    DL_SHAW_BOOL_TRUE  = 1U,
    DL_SHAW_BOOL_FALSE = 0U
} DL_SHAW_BOOL;

/**
 * @brief Structure to hold block processing information
 */
typedef struct {
    uint32_t *data;          /*!< Pointer to current data position */
    uint32_t numWords;            /*!< Number of complete words */
    uint32_t numBlocks;           /*!< Number of complete blocks */
    uint32_t numExtraWords;       /*!< Number of extra words (not forming complete block) */
    uint32_t numExtraBytes;       /*!< Number of extra bytes (not forming complete word) */
} DL_SHAW_BlockInfo;

/**
 *  @brief Enable power to the SHAW module
 *
 *  @param[in]  shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_enablePower(SHAW_Regs *shaw)
{
    shaw->GPRCM.PWREN = (SHAW_PWREN_KEY_UNLOCK_W | SHAW_PWREN_ENABLE_ENABLE);

    /* Required delay after power enable */
    __asm(" NOP ");
    __asm(" NOP ");
    __asm(" NOP ");
    __asm(" NOP ");
}

/**
 *  @brief Disable power to the SHAW module
 *
 *  @param[in]  shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_disablePower(SHAW_Regs *shaw)
{
    shaw->GPRCM.PWREN = (SHAW_PWREN_KEY_UNLOCK_W | SHAW_PWREN_ENABLE_DISABLE);
}

/**
 *  @brief Check if SHAW module power is enabled
 *
 *  @param[in]  shaw  Pointer to the register overlay for the peripheral
 *
 *  @return true if power is enabled, false if disabled
 */
__STATIC_INLINE DL_SHAW_BOOL DL_SHAW_isPowerEnabled(SHAW_Regs *shaw)
{
    return ( DL_SHAW_BOOL ) ((shaw->GPRCM.PWREN & SHAW_PWREN_ENABLE_MASK) == 
            SHAW_PWREN_ENABLE_ENABLE);
}

/**
 *  @brief Reset the SHAW module
 *
 *  @param[in]  shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_reset(SHAW_Regs *shaw)
{
    /* Clear Reset Sticky bit in STAT */
    shaw->GPRCM.RSTCTL = (SHAW_RSTCTL_KEY_UNLOCK_W | 
                          SHAW_RSTCTL_RESETSTKYCLR_MASK);

    /* Assert Reset */
    shaw->GPRCM.RSTCTL |= (SHAW_RSTCTL_KEY_UNLOCK_W | 
                          SHAW_RSTCTL_RESETASSERT_ASSERT);

    /* Required delay after reset */
    __asm(" NOP ");
    __asm(" NOP ");
    __asm(" NOP ");
    __asm(" NOP ");
}

/**
 *  @brief Check if SHAW module was reset
 *
 *  @param[in]  shaw  Pointer to the register overlay for the peripheral
 *
 *  @return true if peripheral was reset, false otherwise
 */
__STATIC_INLINE DL_SHAW_BOOL DL_SHAW_isReset(SHAW_Regs *shaw)
{
    return ( DL_SHAW_BOOL ) ((shaw->GPRCM.STAT & SHAW_STAT_RESETSTKY_MASK) == 
            SHAW_STAT_RESETSTKY_RESET);
}

/**
 * @brief Enable SHA Output Ready interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_enableOutputReadyInterrupt(SHAW_Regs *shaw)
{
    shaw->SHAW.SHAW_PUB0_IMASK |= SHAW_SHAW_PUB0_IMASK_OUTPUTRDY_MASK;
}

/**
 * @brief Enable SHA DMA Trigger interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_enableDMATriggerInterrupt(SHAW_Regs *shaw)
{
    shaw->SHAW.SHAW_PUB1_IMASK |= SHAW_SHAW_PUB1_IMASK_DMA_START_TRIGGER_MASK;
}

/**
 * @brief Disable SHA Output Ready interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_disableOutputReadyInterrupt(SHAW_Regs *shaw)
{
    shaw->SHAW.SHAW_PUB0_IMASK &= ~SHAW_SHAW_PUB0_IMASK_OUTPUTRDY_MASK;
}

/**
 * @brief Disable SHA DMA Trigger interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_disableDMATriggerInterrupt(SHAW_Regs *shaw)
{
    shaw->SHAW.SHAW_PUB1_IMASK &= ~SHAW_SHAW_PUB1_IMASK_DMA_START_TRIGGER_MASK;
}

/**
 * @brief Get SHA Output Ready interrupt status
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 * @return uint32_t Status of Output Ready interrupt
 */
__STATIC_INLINE uint32_t DL_SHAW_getOutputReadyInterruptStatus(SHAW_Regs *shaw)
{
    return (shaw->SHAW.SHAW_PUB0_MIS & SHAW_SHAW_PUB0_MIS_OUTPUTRDY_MASK);
}

/**
 * @brief Get SHA DMA Trigger interrupt status
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 * @return uint32_t Status of DMA Trigger interrupt
 */
__STATIC_INLINE uint32_t DL_SHAW_getDMATriggerInterruptStatus(SHAW_Regs *shaw)
{
    return (shaw->SHAW.SHAW_PUB1_MIS & SHAW_SHAW_PUB1_MIS_DMA_START_TRIGGER_MASK);
}

/**
 * @brief Clear SHA Output Ready interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_clearOutputReadyInterrupt(SHAW_Regs *shaw)
{
    shaw->SHAW.SHAW_PUB0_ICLR = SHAW_SHAW_PUB0_ICLR_OUTPUTRDY_MASK;
}

/**
 * @brief Clear SHA DMA Trigger interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_clearDMATriggerInterrupt(SHAW_Regs *shaw)
{
    shaw->SHAW.SHAW_PUB1_ICLR = SHAW_SHAW_PUB1_ICLR_DMA_START_TRIGGER_MASK;
}

/**
 * @brief Get SHA Output Ready raw interrupt status
 *
 * Checks if Output Ready interrupt is pending, regardless of whether it is enabled.
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 * @return uint32_t Raw status of Output Ready interrupt
 */
__STATIC_INLINE uint32_t DL_SHAW_getOutputReadyRawInterruptStatus(SHAW_Regs *shaw)
{
    return (shaw->SHAW.SHAW_PUB0_RIS & SHAW_SHAW_PUB0_RIS_OUTPUTRDY_MASK);
}

/**
 * @brief Get SHA DMA Trigger raw interrupt status
 *
 * Checks if DMA Trigger interrupt is pending, regardless of whether it is enabled.
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 * @return uint32_t Raw status of DMA Trigger interrupt
 */
__STATIC_INLINE uint32_t DL_SHAW_getDMATriggerRawInterruptStatus(SHAW_Regs *shaw)
{
    return (shaw->SHAW.SHAW_PUB1_RIS & SHAW_SHAW_PUB1_RIS_DMA_START_TRIGGER_MASK);
}

/**
 * @brief Get highest priority pending interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 *
 * @return One of @ref DL_SHAW_IIDX
 */
__STATIC_INLINE DL_SHAW_IIDX DL_SHAW_getPendingInterrupt(SHAW_Regs *shaw)
{
    return (DL_SHAW_IIDX)(shaw->SHAW.SHAW_PUB0_IIDX);
}

/**
 * @brief Get highest priority pending DMA interrupt
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 *
 * @return One of @ref DL_SHAW_IIDX
 */
__STATIC_INLINE DL_SHAW_IIDX DL_SHAW_getPendingDMAInterrupt(SHAW_Regs *shaw)
{
    return (DL_SHAW_IIDX)(shaw->SHAW.SHAW_PUB1_IIDX);
}

/**
 * @brief Check if input buffer is ready for more data
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 *
 * @return true if input buffer is ready, false otherwise
 */
__STATIC_INLINE DL_SHAW_BOOL DL_SHAW_isInputReady(SHAW_Regs *shaw)
{
    return ( DL_SHAW_BOOL ) ((shaw->SHAW.EIP59_IO_BUF_CTRL_STAT & 
            (DL_SHAW_IO_BUF_CTRL_DATA_IN_AV | 
             DL_SHAW_IO_BUF_CTRL_LENGTH_IN_AV | 
             DL_SHAW_IO_BUF_CTRL_MODE_IN_AV| SHAW_EIP59_IO_BUF_CTRL_STAT_IDIGEST_IN_AV_MASK | 
             SHAW_EIP59_IO_BUF_CTRL_STAT_ODIGEST_IN_AV_MASK | 
             SHAW_EIP59_IO_BUF_CTRL_STAT_PAD_MESSAGE_MASK | SHAW_EIP59_IO_BUF_CTRL_STAT_GET_DIGEST_MASK)) == 0U);
}

/**
 * @brief Check if output data is ready
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 *
 * @return true if output is ready, false otherwise
 */
__STATIC_INLINE DL_SHAW_BOOL DL_SHAW_isOutputReady(SHAW_Regs *shaw)
{
    return ( DL_SHAW_BOOL ) ((shaw->SHAW.EIP59_IO_BUF_CTRL_STAT & DL_SHAW_IO_BUF_CTRL_OUTBUF_FULL) != 0U);
}

/**
 * @brief Set the mode configuration
 *
 * @param[in] shaw      Pointer to the register overlay for the peripheral
 * @param[in] algorithm SHA algorithm selection
 * @param[in] mode      Operation mode (HASH/HMAC)
 */
__STATIC_INLINE void DL_SHAW_setMode(SHAW_Regs *shaw, 
    DL_SHAW_ALGORITHM algorithm, DL_SHAW_MODE mode)
{
    uint32_t modeConfig = DL_SHAW_MODE_IN_NEW_HASH_MASK | algorithm | mode;
    
    if (mode == DL_SHAW_MODE_HMAC) {
        modeConfig |= DL_SHAW_MODE_IN_MAC_KEY_MASK;
    }
    
    shaw->SHAW.EIP59_MODE_IN = modeConfig;
}

/**
 * @brief Signals to the SHA module that mode configuration is available
 *
 * @param shaw Pointer to the SHA module registers
 * @return DL_SHAW_STATUS_SUCCESS Operation successful
 */
__STATIC_INLINE DL_SHAW_STATUS DL_SHAW_signalModeAvailable(SHAW_Regs *shaw) {
    shaw->SHAW.EIP59_IO_BUF_CTRL_STAT |= DL_SHAW_IO_BUF_CTRL_MODE_IN_AV;
    return DL_SHAW_STATUS_SUCCESS;
}

/**
 * @brief Sets both mode and length available bits in SHA control register
 *
 * @param shaw Pointer to the SHA module registers
 */
__STATIC_INLINE void DL_SHAW_signalModeAndLengthAvailable(SHAW_Regs *shaw)
{
    shaw->SHAW.EIP59_IO_BUF_CTRL_STAT = 0x0C;
}

/**
 * @brief Signals to the SHA module that length information is available
 *
 * @param shaw Pointer to the SHA module registers
 * @return DL_SHAW_STATUS_SUCCESS Operation successful
 */
__STATIC_INLINE DL_SHAW_STATUS DL_SHAW_signalLengthAvailable(SHAW_Regs *shaw) {
    uint32_t mask = (DL_SHAW_IO_BUF_CTRL_LENGTH_IN_AV);
    shaw->SHAW.EIP59_IO_BUF_CTRL_STAT |= mask;
    return DL_SHAW_STATUS_SUCCESS;
}

/**
 * @brief Signals to the SHA module that input data is available for processing
 *
 * @param shaw Pointer to the SHA module registers
 * @return DL_SHAW_STATUS_SUCCESS Operation successful
 */
__STATIC_INLINE DL_SHAW_STATUS DL_SHAW_signalDataAvailable(SHAW_Regs *shaw) {
    shaw->SHAW.EIP59_IO_BUF_CTRL_STAT |= (DL_SHAW_IO_BUF_CTRL_DATA_IN_AV | DL_SHAW_IO_BUF_CTRL_PAD_MESSAGE);
    return DL_SHAW_STATUS_SUCCESS;
}

/**
 * @brief Start DMA operation
 *
 * @param[in] shaw  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_SHAW_startDMA(SHAW_Regs *shaw)
{
    shaw->SHAW.SHAW_DMA_START_TRIGGER = SHAW_SHAW_DMA_START_TRIGGER_START_TRIGGER_MASK;
}

/* Function prototypes */
/**
 * @brief Initialize the SHA hardware module
 *
 * @param shaw      Pointer to the SHA module registers
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_init(SHAW_Regs *shaw);

/**
 * @brief Configure SHA mode and algorithm
 *
 * @param shaw      Pointer to the SHA module registers
 * @param algorithm SHA algorithm (SHA-224/SHA-256)
 * @param mode      Operation mode (Hash/HMAC)
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_setModeConfig(SHAW_Regs *shaw,
    DL_SHAW_ALGORITHM algorithm, DL_SHAW_MODE mode);

/**
 * @brief Set the total length of data to be processed
 *
 * @param shaw          Pointer to the SHA module registers
 * @param lengthInBytes Total length of input data in bytes
 * @param mode          Operation mode (Hash/HMAC)
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_setDataLength(SHAW_Regs *shaw,
    uint32_t lengthInBytes, DL_SHAW_MODE mode);

/**
 * @brief Write a full block of data (512 bits) to SHA module
 *
 * @param shaw      Pointer to the SHA module registers
 * @param data      Pointer to input data (must be word-aligned)
 * @param length    Length of data in bytes (must be multiple of 64)
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_writeDataBlock(SHAW_Regs *shaw,
    uint32_t *data, uint32_t length);

/**
 * @brief Write a partial block of data to SHA module
 *
 * @param shaw      Pointer to the SHA module registers
 * @param data      Pointer to input data (must be word-aligned)
 * @param numWords  Number of complete 32-bit words
 * @param numBytes  Number of remaining bytes (0-3)
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_writePartialBlock(SHAW_Regs *shaw,
    uint32_t *data, uint32_t numWords, uint32_t numBytes);

/**
 * @brief Calculate block information for data processing
 *
 * @param data      Pointer to input data
 * @param length    Total length of data in bytes
 * @param blockInfo Pointer to structure to store block information
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_getBlockInfo(uint32_t *data,
    uint32_t length, DL_SHAW_BlockInfo *blockInfo);

/**
 * @brief Read the computed digest from SHA module
 *
 * @param shaw      Pointer to the SHA module registers
 * @param digest    Pointer to store the computed hash
 * @param length    Length of digest in bytes
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_readDigest(SHAW_Regs *shaw,
    uint32_t *digest, uint32_t length);

/**
 * @brief Signal last data block consumption to SHA module
 *
 * @param shaw      Pointer to the SHA module registers
 * @param msgType   Type of message (Hash/HMAC inner/outer)
 * @return DL_SHAW_STATUS Success or error code
 */
DL_SHAW_STATUS DL_SHAW_EIPconsumeLastData(SHAW_Regs *shaw,
    DL_SHAW_MSGTYPE msgType);

/**
 * @brief Release the digest registers after reading
 *
 * @param shaw Pointer to the SHA module registers
 */
void DL_SHAW_releaseDigest(SHAW_Regs *shaw);

/**
 * @brief Close the SHA module and release resources
 *
 * @param shaw Pointer to the SHA module registers
 */
void DL_SHAW_close(SHAW_Regs *shaw);