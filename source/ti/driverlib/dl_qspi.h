/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  @file       dl_qspi.h
 *  @brief      QSPI Driver Library
 *  @defgroup   QSPI Serial Peripheral Interface (QSPI)
 *
 *  @anchor ti_dl_dl_qspi_Overview
 *  # Overview
 *
 *  The Serial Peripheral Interface Driver Library allows full configuration of
 *  the MSPM33 QSPI module.
 *  The serial peripheral interface (QSPI) module provides a standardized
 *  serial interface to transfer data between MSPM33 devices and other external
 *  devices with QSPI interface.
 *
 *  <hr>
 ******************************************************************************
 */
/** @addtogroup QSPI
 * @{
 */
#ifndef ti_dl_dl_qspi__include
#define ti_dl_dl_qspi__include

#include <stdbool.h>
#include <stdint.h>

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

#ifdef __MSP_HAS_QSPI__

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */

/** @addtogroup DL_QSPI_CD_MODE
 *  @{
 */
/*!
 * @brief Data mode
 */
#define DL_QSPI_CD_MODE_DATA        (QSPI_CTL1_CDMODE_DATA >> QSPI_CTL1_CDMODE_OFS)

/*!
 * @brief Command mode
 */
#define DL_QSPI_CD_MODE_COMMAND  (QSPI_CTL1_CDMODE_COMMAND >> QSPI_CTL1_CDMODE_OFS)


/** @}*/

/** @addtogroup DL_QSPI_INTERRUPT
 *  @{
 */

/*!
 * @brief DMA done 1 event for transmit interrupt
 */
#define DL_QSPI_INTERRUPT_DMA_DONE_TX    (QSPI_CPU_INT_IMASK_DMA_DONE_TX_SET)

/*!
 * @brief DMA done 1 event for receive interrupt
 */
#define DL_QSPI_INTERRUPT_DMA_DONE_RX    (QSPI_CPU_INT_IMASK_DMA_DONE_RX_SET)
/*!
 * @brief QSPI has finished transfers and changed into idle mode interrupt
 */
#define DL_QSPI_INTERRUPT_IDLE                   (QSPI_CPU_INT_IMASK_IDLE_SET)

/*!
 * @brief Transmit FIFO empty interrupt
 */
#define DL_QSPI_INTERRUPT_TX_EMPTY            (QSPI_CPU_INT_IMASK_TXEMPTY_SET)

/*!
 * @brief Transmit FIFO interrupt
 */
#define DL_QSPI_INTERRUPT_TX                       (QSPI_CPU_INT_IMASK_TX_SET)

/*!
 * @brief Receive FIFO interrupt
 */
#define DL_QSPI_INTERRUPT_RX                       (QSPI_CPU_INT_IMASK_RX_SET)

/*!
 * @brief Receive timeout interrupt
 */
#define DL_QSPI_INTERRUPT_RX_TIMEOUT            (QSPI_CPU_INT_IMASK_RTOUT_SET)

/*!
 * @brief Receive FIFO full interrupt
 */
#define DL_QSPI_INTERRUPT_RX_FULL              (QSPI_CPU_INT_IMASK_RXFULL_SET)

/*!
 * @brief Transmit FIFO underflow interrupt
 */
#define DL_QSPI_INTERRUPT_TX_UNDERFLOW     (QSPI_CPU_INT_IMASK_TXFIFO_UNF_SET)

/*!
 * @brief Parity error
 */
#define DL_QSPI_INTERRUPT_PARITY_ERROR            (QSPI_CPU_INT_IMASK_PER_SET)

/*!
 * @brief Receive FIFO overflow interrupt
 */
#define DL_QSPI_INTERRUPT_RX_OVERFLOW      (QSPI_CPU_INT_IMASK_RXFIFO_OVF_SET)

/** @}*/

/*! @enum DL_QSPI_DMA_IIDX_RX */
typedef enum {
    /*! QSPI interrupt index for enabling QSPI receive as DMA trigger */
    DL_QSPI_DMA_IIDX_RX_TRIGGER = QSPI_DMA_TRIG_RX_IIDX_STAT_RX_EVT,
    /*! QSPI interrupt index for enabling QSPI receive timeout as DMA trigger */
    DL_QSPI_DMA_IIDX_RX_TIMEOUT_TRIGGER = QSPI_DMA_TRIG_RX_IIDX_STAT_RTOUT_EVT
} DL_QSPI_DMA_IIDX_RX;

/*! @enum DL_QSPI_DMA_IIDX_TX */
typedef enum {
    /*! QSPI interrupt index for enabling QSPI transmit as DMA trigger */
    DL_QSPI_DMA_IIDX_TX_TRIGGER = QSPI_DMA_TRIG_TX_IIDX_STAT_TX_EVT
} DL_QSPI_DMA_IIDX_TX;

/** @addtogroup DL_QSPI_DMA_INTERRUPT_RX
 *  @{
 */
/*!
 * @brief QSPI interrupt for enabling QSPI receive as DMA trigger
 */
#define DL_QSPI_DMA_INTERRUPT_RX               (QSPI_DMA_TRIG_RX_IMASK_RX_SET)

/*!
 * @brief QSPI interrupt for enabling QSPI receive timeout as DMA trigger
 */
#define DL_QSPI_DMA_INTERRUPT_RX_TIMEOUT       (QSPI_DMA_TRIG_RX_IMASK_RTOUT_SET)

/** @}*/

/*!
 * @brief QSPI interrupt for enabling QSPI transmit as DMA trigger
 */
#define DL_QSPI_DMA_INTERRUPT_TX               (QSPI_DMA_TRIG_TX_IMASK_TX_SET)

/* clang-format on */

/*! @enum DL_QSPI_PARITY */
typedef enum {
    /*! Use even parity, enable transmit and receive parity */
    DL_QSPI_PARITY_EVEN =
        (QSPI_CTL1_PES_ENABLE | QSPI_CTL1_PREN_ENABLE | QSPI_CTL1_PTEN_ENABLE),
    /*! Use odd parity, enable transmit and receive parity  */
    DL_QSPI_PARITY_ODD =
        (QSPI_CTL1_PES_DISABLE | QSPI_CTL1_PREN_ENABLE | QSPI_CTL1_PTEN_ENABLE),
    /*! Disable receive and transmit parity */
    DL_QSPI_PARITY_NONE = (QSPI_CTL1_PREN_DISABLE | QSPI_CTL1_PTEN_DISABLE)
} DL_QSPI_PARITY;

/*! @enum DL_QSPI_FRAME_FORMAT */
typedef enum {
    /*! Motorola 3 Wire with Polarity 0, Phase 0 */
    DL_QSPI_FRAME_FORMAT_MOTO3_POL0_PHA0 =
        (QSPI_CTL0_SPO_LOW | QSPI_CTL0_SPH_FIRST | QSPI_CTL0_FRF_MOTOROLA_3WIRE),
    /*! Motorola 3 Wire with Polarity 0, Phase 1 */
    DL_QSPI_FRAME_FORMAT_MOTO3_POL0_PHA1 =
        (QSPI_CTL0_SPO_LOW | QSPI_CTL0_SPH_SECOND | QSPI_CTL0_FRF_MOTOROLA_3WIRE),
    /*! Motorola 3 Wire with Polarity 1, Phase 0 */
    DL_QSPI_FRAME_FORMAT_MOTO3_POL1_PHA0 =
        (QSPI_CTL0_SPO_HIGH | QSPI_CTL0_SPH_FIRST | QSPI_CTL0_FRF_MOTOROLA_3WIRE),
    /*! Motorola 3 Wire with Polarity 1, Phase 1 */
    DL_QSPI_FRAME_FORMAT_MOTO3_POL1_PHA1 =
        (QSPI_CTL0_SPO_HIGH | QSPI_CTL0_SPH_SECOND |
            QSPI_CTL0_FRF_MOTOROLA_3WIRE),
    /*! Motorola 4 Wire with Polarity 0, Phase 0 */
    DL_QSPI_FRAME_FORMAT_MOTO4_POL0_PHA0 =
        (QSPI_CTL0_SPO_LOW | QSPI_CTL0_SPH_FIRST | QSPI_CTL0_FRF_MOTOROLA_4WIRE),
    /*! Motorola 4 Wire with Polarity 0, Phase 1 */
    DL_QSPI_FRAME_FORMAT_MOTO4_POL0_PHA1 =
        (QSPI_CTL0_SPO_LOW | QSPI_CTL0_SPH_SECOND | QSPI_CTL0_FRF_MOTOROLA_4WIRE),
    /*! Motorola 4 Wire with Polarity 1, Phase 0 */
    DL_QSPI_FRAME_FORMAT_MOTO4_POL1_PHA0 =
        (QSPI_CTL0_SPO_HIGH | QSPI_CTL0_SPH_FIRST | QSPI_CTL0_FRF_MOTOROLA_4WIRE),
    /*! Motorola 4 Wire with Polarity 1, Phase 1 */
    DL_QSPI_FRAME_FORMAT_MOTO4_POL1_PHA1 =
        (QSPI_CTL0_SPO_HIGH | QSPI_CTL0_SPH_SECOND |
            QSPI_CTL0_FRF_MOTOROLA_4WIRE),
    /*! TI Sync Frame Format */
    DL_QSPI_FRAME_FORMAT_TI_SYNC = (QSPI_CTL0_FRF_TI_SYNC),
} DL_QSPI_FRAME_FORMAT;

/*! @enum DL_QSPI_MODE */
typedef enum {
    /*! Controller mode */
    DL_QSPI_MODE_CONTROLLER = (QSPI_CTL1_CP_ENABLE),
    /*! Peripheral mode */
    DL_QSPI_MODE_PERIPHERAL = (QSPI_CTL1_CP_DISABLE)
} DL_QSPI_MODE;

typedef enum {
    /*! Direct mode */
    DL_QSPI_PORT_DIRECT = (QSPI_QSPICTL0_QSPIMODE_DIRECT_PORT),
    /*! Indirect mode */
    DL_QSPI_PORT_INDIRECT = (QSPI_QSPICTL0_QSPIMODE_PERIPH_PORT)
} DL_QSPI_PORT;

typedef enum {
    /*! 3 Byte address mode */
    DL_QSPI_ADDRESS_MODE_3BYTE = (QSPI_QSPICTL0_QSPIADDRMODE__3BYTE_ADDR),
    /*! 4 Byte address mode */
    DL_QSPI_ADDRESS_MODE_4BYTE = (QSPI_QSPICTL0_QSPIADDRMODE__4BYTE_ADDR)
} DL_QSPI_ADDRESS_MODE;

typedef enum {
    DL_QSPI_BUS_FORMAT_SPI_FULL_DUPLEX = (QSPI_QSPICTL0_QSPIFORMAT_LEGACY_SPI_FULLDUPLEX),
    DL_QSPI_BUS_FORMAT_SPI_HALF_DUPLEX = (QSPI_QSPICTL0_QSPIFORMAT_SPI_HALFDUPLEX),
    DL_QSPI_BUS_FORMAT_SSS_MODE = (QSPI_QSPICTL0_QSPIFORMAT_SSS_MODE),
    DL_QSPI_BUS_FORMAT_SSD_MODE = (QSPI_QSPICTL0_QSPIFORMAT_SSD_MODE),
    DL_QSPI_BUS_FORMAT_SDD_MODE = (QSPI_QSPICTL0_QSPIFORMAT_SDD_MODE),
    DL_QSPI_BUS_FORMAT_SSQ_MODE = (QSPI_QSPICTL0_QSPIFORMAT_SSQ_MODE),
    DL_QSPI_BUS_FORMAT_SQQ_MODE = (QSPI_QSPICTL0_QSPIFORMAT_SQQ_MODE),
    DL_QSPI_BUS_FORMAT_QQQ_MODE = (QSPI_QSPICTL0_QSPIFORMAT_QQQ_MODE),
} DL_QSPI_BUS_FORMAT;

typedef enum {
    DL_QSPI_WORD_SIZE_8 = (QSPI_QSPICTL0_QSPIDSIZE__8BITDSIZE),
    DL_QSPI_WORD_SIZE_16 = (QSPI_QSPICTL0_QSPIDSIZE__16BITDSIZE),
    DL_QSPI_WORD_SIZE_32 = (QSPI_QSPICTL0_QSPIDSIZE__32BITDSIZE)
} DL_QSPI_WORD_SIZE;

typedef enum {
    DL_QSPI_DATA_SAMPLE_DELAY_0 = (QSPI_QSPITIMING_DATASAMPLDLY_NODELAY),
    DL_QSPI_DATA_SAMPLE_DELAY_1 = (QSPI_QSPITIMING_DATASAMPLDLY__1CLKDELAY)
} DL_QSPI_DATA_SAMPLE_DELAY;

typedef enum {
    DL_QSPI_CHIP_SELECT_ASSERT_DELAY_0P5 = (QSPI_QSPITIMING_CSASSERTDLY__0P5SCLKDELAY),
    DL_QSPI_CHIP_SELECT_ASSERT_DELAY_1P5 = (QSPI_QSPITIMING_CSASSERTDLY__1P5SCLKDELAY)
} DL_QSPI_CHIP_SELECT_ASSERT_DELAY;

typedef enum {
    DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_0P5 = (QSPI_QSPITIMING_CSDEASSERTDLY__0P5SCLKDELAY),
    DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_1P5 = (QSPI_QSPITIMING_CSDEASSERTDLY__1P5SCLKDELAY)
} DL_QSPI_CHIP_SELECT_DEASSERT_DELAY;

/*! @enum DL_QSPI_BIT_ORDER */
typedef enum {
    /*! MSB First */
    DL_QSPI_BIT_ORDER_MSB_FIRST = (QSPI_CTL1_MSB_ENABLE),
    /*! LSB First */
    DL_QSPI_BIT_ORDER_LSB_FIRST = (QSPI_CTL1_MSB_DISABLE)
} DL_QSPI_BIT_ORDER;

/*! @enum DL_QSPI_DATA_SIZE */
typedef enum {
    /*! Data size 4 bits */
    DL_QSPI_DATA_SIZE_4 = (QSPI_CTL0_DSS_DSS_4),
    /*! Data size 5 bits */
    DL_QSPI_DATA_SIZE_5 = (QSPI_CTL0_DSS_DSS_5),
    /*! Data size 6 bits */
    DL_QSPI_DATA_SIZE_6 = (QSPI_CTL0_DSS_DSS_6),
    /*! Data size 7 bits */
    DL_QSPI_DATA_SIZE_7 = (QSPI_CTL0_DSS_DSS_7),
    /*! Data size 8 bits */
    DL_QSPI_DATA_SIZE_8 = (QSPI_CTL0_DSS_DSS_8),
    /*! Data size 9 bits */
    DL_QSPI_DATA_SIZE_9 = (QSPI_CTL0_DSS_DSS_9),
    /*! Data size 10 bits */
    DL_QSPI_DATA_SIZE_10 = (QSPI_CTL0_DSS_DSS_10),
    /*! Data size 11 bits */
    DL_QSPI_DATA_SIZE_11 = (QSPI_CTL0_DSS_DSS_11),
    /*! Data size 12 bits */
    DL_QSPI_DATA_SIZE_12 = (QSPI_CTL0_DSS_DSS_12),
    /*! Data size 13 bits */
    DL_QSPI_DATA_SIZE_13 = (QSPI_CTL0_DSS_DSS_13),
    /*! Data size 14 bits */
    DL_QSPI_DATA_SIZE_14 = (QSPI_CTL0_DSS_DSS_14),
    /*! Data size 15 bits */
    DL_QSPI_DATA_SIZE_15 = (QSPI_CTL0_DSS_DSS_15),
    /*! Data size 16 bits */
    DL_QSPI_DATA_SIZE_16 = (QSPI_CTL0_DSS_DSS_16),
} DL_QSPI_DATA_SIZE;

/*! @enum DL_QSPI_CHIP_SELECT */
typedef enum {
    /*! Chip Select 0 */
    DL_QSPI_CHIP_SELECT_0 = (QSPI_CTL0_CSSEL_CSSEL_0),
    /*! Chip Select 1 */
    DL_QSPI_CHIP_SELECT_1 = (QSPI_CTL0_CSSEL_CSSEL_1),
    /*! Chip Select 2 */
    DL_QSPI_CHIP_SELECT_2 = (QSPI_CTL0_CSSEL_CSSEL_2),
    /*! Chip Select 3 */
    DL_QSPI_CHIP_SELECT_3 = (QSPI_CTL0_CSSEL_CSSEL_3),
    /*! No chip select */
    DL_QSPI_CHIP_SELECT_NONE = (0)
} DL_QSPI_CHIP_SELECT;

/*! @enum DL_QSPI_TX_FIFO_LEVEL */
typedef enum {
    /*! Interrupt triggers when TX FIFO <= 3/4 empty */
    DL_QSPI_TX_FIFO_LEVEL_3_4_EMPTY = QSPI_IFLS_TXIFLSEL_LVL_3_4,
    /*! Interrupt triggers when TX FIFO <= 1/2 empty (default) */
    DL_QSPI_TX_FIFO_LEVEL_1_2_EMPTY = QSPI_IFLS_TXIFLSEL_LVL_1_2,
    /*! Interrupt triggers when TX FIFO <= 1/4 empty */
    DL_QSPI_TX_FIFO_LEVEL_1_4_EMPTY = QSPI_IFLS_TXIFLSEL_LVL_1_4,
    /*! Interrupt triggers when TX FIFO is empty */
    DL_QSPI_TX_FIFO_LEVEL_EMPTY = QSPI_IFLS_TXIFLSEL_LVL_EMPTY,
    /*! Interrupt triggers when TX FIFO has >= 1 frame free. Should be used with DMA */
    DL_QSPI_TX_FIFO_LEVEL_ONE_FRAME = QSPI_IFLS_TXIFLSEL_LEVEL_1,
    DL_QSPI_TX_FIFO_LEVEL_RES4 = QSPI_IFLS_TXIFLSEL_LVL_RES4,
    DL_QSPI_TX_FIFO_LEVEL_RES6 = QSPI_IFLS_TXIFLSEL_LVL_RES6,
    DL_QSPI_TX_FIFO_LEVEL_OFF = QSPI_IFLS_TXIFLSEL_LVL_OFF
} DL_QSPI_TX_FIFO_LEVEL;

/*! @enum DL_QSPI_RX_FIFO_LEVEL */
typedef enum {
    /*! Interrupt triggers when RX FIFO contains >= 1 frame. Should be used with DMA */
    DL_QSPI_RX_FIFO_LEVEL_ONE_FRAME = QSPI_IFLS_RXIFLSEL_LEVEL_1,
    /*! Interrupt triggers when RX FIFO is full */
    DL_QSPI_RX_FIFO_LEVEL_FULL = QSPI_IFLS_RXIFLSEL_LVL_FULL,
    /*! Interrupt triggers when RX FIFO >= 3/4 full */
    DL_QSPI_RX_FIFO_LEVEL_3_4_FULL = QSPI_IFLS_RXIFLSEL_LVL_3_4,
    /*! Interrupt triggers when RX FIFO >= 1/2 full (default) */
    DL_QSPI_RX_FIFO_LEVEL_1_2_FULL = QSPI_IFLS_RXIFLSEL_LVL_1_2,
    /*! Interrupt triggers when RX FIFO >= 1/4 full */
    DL_QSPI_RX_FIFO_LEVEL_1_4_FULL = QSPI_IFLS_RXIFLSEL_LVL_1_4,
    DL_QSPI_RX_FIFO_LEVEL_RES4 = QSPI_IFLS_RXIFLSEL_LVL_RES4,
    DL_QSPI_RX_FIFO_LEVEL_RES6 = QSPI_IFLS_RXIFLSEL_LVL_RES6,
    DL_QSPI_RX_FIFO_LEVEL_OFF = QSPI_IFLS_RXIFLSEL_LVL_OFF
} DL_QSPI_RX_FIFO_LEVEL;

/*! @enum DL_QSPI_IIDX */
typedef enum {

    /*! QSPI interrupt index for DMA Done 1 event for transmit */
    DL_QSPI_IIDX_DMA_DONE_TX = QSPI_CPU_INT_IIDX_STAT_DMA_DONE_TX_EVT,
    /*! QSPI interrupt index for DMA Done 1 event for receive */
    DL_QSPI_IIDX_DMA_DONE_RX = QSPI_CPU_INT_IIDX_STAT_DMA_DONE_RX_EVT,
    /*! QSPI interrupt index for QSPI to signal it has finished transfers and
     * changed into idle mode */

    DL_QSPI_IIDX_IDLE = QSPI_CPU_INT_IIDX_STAT_IDLE_EVT,
    /*! QSPI interrupt index for transmit FIFO empty */
    DL_QSPI_IIDX_TX_EMPTY = QSPI_CPU_INT_IIDX_STAT_TX_EMPTY,
    /*! QSPI interrupt index for transmit FIFO */
    DL_QSPI_IIDX_TX = QSPI_CPU_INT_IIDX_STAT_TX_EVT,
    /*! QSPI interrupt index for receive FIFO */
    DL_QSPI_IIDX_RX = QSPI_CPU_INT_IIDX_STAT_RX_EVT,
    /*! QSPI interrupt index for receive timeout */
    DL_QSPI_IIDX_RX_TIMEOUT = QSPI_CPU_INT_IIDX_STAT_RTOUT_EVT,

    /*! QSPI interrupt index for receive FIFO full  */
    DL_QSPI_IIDX_RX_FULL = QSPI_CPU_INT_IIDX_STAT_RXFULL_EVT,
    /*! QSPI interrupt index for transmit FIFO underflow  */
    DL_QSPI_IIDX_TX_UNDERFLOW = QSPI_CPU_INT_IIDX_STAT_TXFIFO_UNF_EVT,

    /*! QSPI interrupt index for parity error */
    DL_QSPI_IIDX_PARITY_ERROR = QSPI_CPU_INT_IIDX_STAT_PER_EVT,
    /*! QSPI interrupt index for receive FIFO overflow */
    DL_QSPI_IIDX_RX_OVERFLOW = QSPI_CPU_INT_IIDX_STAT_RXFIFO_OFV_EVT
} DL_QSPI_IIDX;

/*! @enum DL_QSPI_CLOCK_DIVIDE_RATIO */
typedef enum {
    /*! QSPI source clock divide ratio set to 1 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_1 = QSPI_CLKDIV_RATIO_DIV_BY_1,
    /*! QSPI source clock divide ratio set to 2 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_2 = QSPI_CLKDIV_RATIO_DIV_BY_2,
    /*! QSPI source clock divide ratio set to 3 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_3 = QSPI_CLKDIV_RATIO_DIV_BY_3,
    /*! QSPI source clock divide ratio set to 4 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_4 = QSPI_CLKDIV_RATIO_DIV_BY_4,
    /*! QSPI source clock divide ratio set to 5 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_5 = QSPI_CLKDIV_RATIO_DIV_BY_5,
    /*! QSPI source clock divide ratio set to 6 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_6 = QSPI_CLKDIV_RATIO_DIV_BY_6,
    /*! QSPI source clock divide ratio set to 7 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_7 = QSPI_CLKDIV_RATIO_DIV_BY_7,
    /*! QSPI source clock divide ratio set to 8 */
    DL_QSPI_CLOCK_DIVIDE_RATIO_8 = QSPI_CLKDIV_RATIO_DIV_BY_8
} DL_QSPI_CLOCK_DIVIDE_RATIO;

/*! @enum DL_QSPI_CLOCK */
typedef enum {
    /*! Selects BUSCLK as the clock source */
    DL_QSPI_CLOCK_BUSCLK = QSPI_CLKSEL_SYSCLK_SEL_ENABLE,
    /*! Selects MFCLK as the clock source */
    DL_QSPI_CLOCK_MFCLK = QSPI_CLKSEL_MFCLK_SEL_ENABLE,
    /*! Selects LFCLK as the clock source */
    DL_QSPI_CLOCK_LFCLK = QSPI_CLKSEL_LFCLK_SEL_ENABLE
} DL_QSPI_CLOCK;

/*!
 *  @brief  Configuration struct for @ref DL_QSPI_init.
 */
typedef struct {
    /*! The controller/peripheral mode configuration. One of @ref DL_QSPI_MODE */
    DL_QSPI_MODE mode;

    /*!
     *  The frame format to use for data transfer. One of @ref
     *  DL_QSPI_FRAME_FORMAT
     */
    DL_QSPI_FRAME_FORMAT frameFormat;

    /*!
     *  The parity configuration to use for data transfer. One of @ref
     *  DL_QSPI_PARITY.
     */
    DL_QSPI_PARITY parity;

    /*! The size of the data transfer. One of @ref DL_QSPI_DATA_SIZE */
    DL_QSPI_DATA_SIZE dataSize;

    /*! The order of bits during data transfer. One of @ref DL_QSPI_BIT_ORDER */
    DL_QSPI_BIT_ORDER bitOrder;

    /*! The pin to use for chip select. Used in Controller or Peripheral modes
     *  with Motorola 4-Wire or TI Sync frame formats. One of
     *  @ref DL_QSPI_CHIP_SELECT.
     */
    DL_QSPI_CHIP_SELECT chipSelectPin;

} DL_QSPI_Config;

/*!
 *  @brief  Configuration struct for @ref DL_QSPI_setClockConfig.
 */
typedef struct {
    /*! Selects QSPI module clock source @ref DL_QSPI_CLOCK */
    DL_QSPI_CLOCK clockSel;

    /*! Selects the divide ratio. One of @ref DL_QSPI_CLOCK_DIVIDE_RATIO */
    DL_QSPI_CLOCK_DIVIDE_RATIO divideRatio;

} DL_QSPI_ClockConfig;

/**
 * @brief Configuration structure to backup QSPI peripheral state before going
 *        to STOP/STANDBY mode. Not required after PG 1.0 silicon. Used by
 *        @ref DL_QSPI_saveConfiguration and @ref DL_QSPI_restoreConfiguration
 */
typedef struct {
    /*! Combination of basic QSPI control configurations that are
     *  compressed to a single word as they are stored in the QSPI
     *  registers See @ref DL_QSPI_init for how the peripheral control word 0
     *  is created. */
    uint32_t controlWord0;

    /*! Combination of basic QSPI control configurations that are
     *  compressed to a single word as they are stored in the QSPI
     *  registers. See @ref DL_QSPI_init for how the peripheral control word 1
     *  is created. */
    uint32_t controlWord1;

    /*! Combination of serial clock divider and delayed sampling settings
     *  compressed to a single word as they are stored in the QSPI registers. */
    uint32_t clockControl;

    /*! QSPI module clock source. One of  @ref DL_QSPI_CLOCK */
    uint32_t clockSel;

    /*! QSPI clock divider. One of @ref DL_QSPI_CLOCK_DIVIDE_RATIO */
    uint32_t divideRatio;

    /*! Combination of QSPI interrupt FIFO level select configurations that are
     *  compressed to a single word as they are stored in the QSPI
     *  registers. */
    uint32_t interruptFifoLevelSelectWord;

    /*! QSPI interrupt status for EVENT0.
     *  Bitwise OR of @ref DL_QSPI_INTERRUPT */
    uint32_t interruptMask0;

    /*! QSPI interrupt status for EVENT1.
     *  Bitwise OR of @ref DL_QSPI_DMA_INTERRUPT_RX */
    uint32_t interruptMask1;

    /*! QSPI interrupt status for EVENT2.
     *  Bitwise OR of @ref DL_QSPI_DMA_INTERRUPT_TX */
    uint32_t interruptMask2;

    /*! Boolean flag indicating whether or not a valid configuration structure
     *  exists. Should not be modified by the user. */
    bool backupRdy;
} DL_QSPI_backupConfig;

/**
 *  @brief      Initialize the QSPI peripheral
 *
 *  Initializes all the common configurable options for the QSPI peripheral. Any
 *  other custom configuration can be done after calling this API. The QSPI is
 *  not enabled in this API.
 *
 *  @param[in]  qspi     Pointer to the register overlay for the peripheral
 *  @param[in]  config  Configuration for QSPI peripheral
 */
void DL_QSPI_init(QSPI_Regs *qspi, DL_QSPI_Config *config);

/**
 * @brief Enables power on QSPI module
 *
 * @param qspi        Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_enablePower(QSPI_Regs *qspi)
{
    qspi->GPRCM.PWREN = (QSPI_PWREN_KEY_UNLOCK_W | QSPI_PWREN_ENABLE_ENABLE);
}

/**
 * @brief Disables power on qspi module
 *
 * @param qspi        Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_disablePower(QSPI_Regs *qspi)
{
    qspi->GPRCM.PWREN = (QSPI_PWREN_KEY_UNLOCK_W | QSPI_PWREN_ENABLE_DISABLE);
}

/**
 * @brief Returns if  power on qspi module
 *
 * @param qspi        Pointer to the register overlay for the peripheral
 *
 * @return true if power is enabled
 * @return false if power is disabled
 */
__STATIC_INLINE bool DL_QSPI_isPowerEnabled(QSPI_Regs *qspi)
{
    return (
        (qspi->GPRCM.PWREN & QSPI_PWREN_ENABLE_MASK) == QSPI_PWREN_ENABLE_ENABLE);
}

/**
 * @brief Resets qspi peripheral
 *
 * @param qspi        Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_reset(QSPI_Regs *qspi)
{
    qspi->GPRCM.RSTCTL =
        (QSPI_RSTCTL_KEY_UNLOCK_W | QSPI_RSTCTL_RESETSTKYCLR_CLR |
            QSPI_RSTCTL_RESETASSERT_ASSERT);
}

/**
 * @brief Returns if qspi peripheral was reset
 *
 * @param qspi        Pointer to the register overlay for the peripheral
 *
 * @return true if peripheral was reset
 * @return false if peripheral wasn't reset
 *
 */
__STATIC_INLINE bool DL_QSPI_isReset(QSPI_Regs *qspi)
{
    return ((qspi->GPRCM.STAT & QSPI_GPRCM_STAT_RESETSTKY_MASK) ==
            QSPI_GPRCM_STAT_RESETSTKY_RESET);
}

/**
 *  @brief      Enable the QSPI peripheral
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_enable(QSPI_Regs *qspi)
{
    qspi->CTL1 |= QSPI_CTL1_ENABLE_ENABLE;
}

/**
 *  @brief      Checks if the QSPI peripheral is enabled
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     Returns the enabled status of the QSPI
 *
 *  @retval     true  The QSPI peripheral is enabled
 *  @retval     false The QSPI peripheral is disabled
 */
__STATIC_INLINE bool DL_QSPI_isEnabled(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_ENABLE_MASK) == QSPI_CTL1_ENABLE_ENABLE);
}

/**
 *  @brief      Disable the QSPI peripheral
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_disable(QSPI_Regs *qspi)
{
    qspi->CTL1 &= ~(QSPI_CTL1_ENABLE_MASK);
}

/**
 *  @brief      Configure QSPI source clock
 *
 *  @param[in]  qspi    Pointer to the register overlay for the
 *                      peripheral
 *  @param[in]  config  Pointer to the clock configuration struct
 *                       @ref DL_QSPI_ClockConfig.
 */
void DL_QSPI_setClockConfig(QSPI_Regs *qspi, DL_QSPI_ClockConfig *config);

/**
 *  @brief      Get QSPI source clock configuration
 *
 *  @param[in]  qspi    Pointer to the register overlay for the
 *                      peripheral
 *  @param[in]  config  Pointer to the clock configuration struct
 *                      @ref DL_QSPI_ClockConfig.
 */
void DL_QSPI_getClockConfig(QSPI_Regs *qspi, DL_QSPI_ClockConfig *config);

/**
 *  @brief      Checks if the QSPI is busy transmitting
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     Returns the busy status of the QSPI
 *
 *  @retval     true  The QSPI is transmitting
 *  @retval     false The QSPI is idle
 */
__STATIC_INLINE bool DL_QSPI_isBusy(QSPI_Regs *qspi)
{
    return ((qspi->STAT & QSPI_STAT_BUSY_MASK) == QSPI_STAT_BUSY_ACTIVE);
}

/**
 *  @brief      Checks if the TX FIFO is empty
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     Returns the empty status of the TX FIFO
 *
 *  @retval     true  The TX FIFO is empty
 *  @retval     false The TX FIFO is not empty
 */
__STATIC_INLINE bool DL_QSPI_isTXFIFOEmpty(QSPI_Regs *qspi)
{
    return ((qspi->STAT & QSPI_STAT_TFE_MASK) == QSPI_STAT_TFE_EMPTY);
}

/**
 *  @brief      Checks if the TX FIFO is full
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     Returns the full status of the TX FIFO
 *
 *  @retval     true  The TX FIFO is full
 *  @retval     false The TX FIFO is not full
 */
__STATIC_INLINE bool DL_QSPI_isTXFIFOFull(QSPI_Regs *qspi)
{
    return ((qspi->STAT & QSPI_STAT_TNF_MASK) == QSPI_STAT_TNF_FULL);
}

/**
 *  @brief      Checks if the RX FIFO is empty
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     Returns the empty status of the RX FIFO
 *
 *  @retval     true  The RX FIFO is empty
 *  @retval     false The RX FIFO is not empty
 */
__STATIC_INLINE bool DL_QSPI_isRXFIFOEmpty(QSPI_Regs *qspi)
{
    return ((qspi->STAT & QSPI_STAT_RFE_MASK) == QSPI_STAT_RFE_EMPTY);
}

/**
 *  @brief      Checks if the RX FIFO is full
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     Returns the full status of the RX FIFO
 *
 *  @retval     true  The RX FIFO is full
 *  @retval     false The RX FIFO is not full
 */
__STATIC_INLINE bool DL_QSPI_isRXFIFOFull(QSPI_Regs *qspi)
{
    return ((qspi->STAT & QSPI_STAT_RNF_MASK) == QSPI_STAT_RNF_FULL);
}

/**
 *  @brief      Sets the parity configuration used for transactions
 *
 *  This API sets the configuration for both receive parity and transmit parity.
 *  This includes which bit is used, and whether even or odd parity is set.
 *
 *  To individually enable or dsiable the receive or transmit parity, refer to
 *  the APIs listed below.
 *
 *  @param[in]  qspi     Pointer to the register overlay for the peripheral
 *  @param[in]  parity  Parity configuration to use. One of @ref DL_QSPI_PARITY.
 *
 *  @sa         DL_QSPI_init
 *  @sa         DL_QSPI_enableReceiveParity
 *  @sa         DL_QSPI_disableReceiveParity
 *  @sa         DL_QSPI_enableTransmitParity
 *  @sa         DL_QSPI_disableTransmitParity
 */
__STATIC_INLINE void DL_QSPI_setParity(QSPI_Regs *qspi, DL_QSPI_PARITY parity)
{
    DL_Common_updateReg(&qspi->CTL1, (uint32_t) parity,
        (QSPI_CTL1_PREN_MASK | QSPI_CTL1_PTEN_MASK | QSPI_CTL1_PES_MASK));
}

/**
 *  @brief      Get the current receive and transmit parity configuration
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The current parity configuration being used
 *
 *  @retval     One of @ref DL_QSPI_PARITY
 */
__STATIC_INLINE DL_QSPI_PARITY DL_QSPI_getParity(QSPI_Regs *qspi)
{
    uint32_t parity = qspi->CTL1 & (QSPI_CTL1_PES_MASK | QSPI_CTL1_PREN_MASK |
                                      QSPI_CTL1_PTEN_MASK);

    return (DL_QSPI_PARITY)(parity);
}

/**
 *  @brief      Enables receive parity
 *
 *  This API only enables receive parity, it does not configure the parity mode
 *  used.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @sa         DL_QSPI_setParity
 */
__STATIC_INLINE void DL_QSPI_enableReceiveParity(QSPI_Regs *qspi)
{
    qspi->CTL1 |= QSPI_CTL1_PREN_ENABLE;
}

/**
 *  @brief      Disables receive parity
 *
 *  This API only disable receive parity, it does not configure the parity mode
 *  used.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @sa         DL_QSPI_setParity
 */
__STATIC_INLINE void DL_QSPI_disableReceiveParity(QSPI_Regs *qspi)
{
    qspi->CTL1 &= ~(QSPI_CTL1_PREN_MASK);
}

/**
 *  @brief      Checks if receive parity is enabled
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     If receive parity is enabled
 *
 *  @retval     true   Receive parity is enabled
 *  @retval     false  Receive parity is disabled
 */
__STATIC_INLINE bool DL_QSPI_isReceiveParityEnabled(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_PREN_MASK) == QSPI_CTL1_PREN_ENABLE);
}

/**
 *  @brief      Enables transmit parity
 *
 *  This API only enables transmit parity, it does not configure the parity mode
 *  used.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @sa         DL_QSPI_setParity
 */
__STATIC_INLINE void DL_QSPI_enableTransmitParity(QSPI_Regs *qspi)
{
    qspi->CTL1 |= QSPI_CTL1_PTEN_ENABLE;
}

/**
 *  @brief      Disables transmit parity
 *
 *  This API only disables transmit parity, it does not configure the parity
 *  mode used.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @sa         DL_QSPI_setParity
 */
__STATIC_INLINE void DL_QSPI_disableTransmitParity(QSPI_Regs *qspi)
{
    qspi->CTL1 &= ~(QSPI_CTL1_PTEN_MASK);
}

/**
 *  @brief      Checks if transmit parity is enabled
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     If transmit parity is enabled
 *
 *  @retval     true   Transmit parity is enabled
 *  @retval     false  Transmit parity is disabled
 */
__STATIC_INLINE bool DL_QSPI_isTransmitParityEnabled(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_PTEN_MASK) == QSPI_CTL1_PTEN_ENABLE);
}

/**
 *  @brief      Set the frame format to use
 *
 *  Configures the frame format to use for transactions. If you are using chip
 *  select you must use one of the Motorola 4 wire frame formats.
 *
 *  @param[in]  qspi          Pointer to the register overlay for the peripheral
 *  @param[in]  frameFormat  Frame format to use. One of @ref
 *                           DL_QSPI_FRAME_FORMAT.
 *
 *  @sa         DL_QSPI_init
 */
__STATIC_INLINE void DL_QSPI_setFrameFormat(
    QSPI_Regs *qspi, DL_QSPI_FRAME_FORMAT frameFormat)
{
    DL_Common_updateReg(&qspi->CTL0, (uint32_t) frameFormat,
        (QSPI_CTL0_FRF_MASK | QSPI_CTL0_SPO_MASK | QSPI_CTL0_SPH_MASK));
}

/**
 *  @brief      Get the frame format configuration
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The current frame format being used
 *
 *  @retval     One of @ref DL_QSPI_FRAME_FORMAT
 */
__STATIC_INLINE DL_QSPI_FRAME_FORMAT DL_QSPI_getFrameFormat(QSPI_Regs *qspi)
{
    uint32_t frameFormat = qspi->CTL0 & (QSPI_CTL0_FRF_MASK | QSPI_CTL0_SPO_MASK |
                                           QSPI_CTL0_SPH_MASK);

    return (DL_QSPI_FRAME_FORMAT)(frameFormat);
}

/**
 *  @brief      Set the size for transfers
 *
 *  @param[in]  qspi       Pointer to the register overlay for the peripheral
 *  @param[in]  dataSize  Number of bits used in a transfer.
 *                        One of @ref DL_QSPI_DATA_SIZE
 *
 *  @sa         DL_QSPI_init
 */
__STATIC_INLINE void DL_QSPI_setDataSize(
    QSPI_Regs *qspi, DL_QSPI_DATA_SIZE dataSize)
{
    DL_Common_updateReg(&qspi->CTL0, (uint32_t) dataSize, QSPI_CTL0_DSS_MASK);
}

/**
 *  @brief      Get the configured size for transfers
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The currently configured size for transfers
 *
 *  @retval     One of @ref DL_QSPI_DATA_SIZE
 */
__STATIC_INLINE DL_QSPI_DATA_SIZE DL_QSPI_getDataSize(QSPI_Regs *qspi)
{
    uint32_t dataSize = qspi->CTL0 & QSPI_CTL0_DSS_MASK;

    return (DL_QSPI_DATA_SIZE)(dataSize);
}

/**
 *  @brief      Set whether the device should be in controller/peripheral mode
 *
 *  @param[in]  qspi   Pointer to the register overlay for the peripheral
 *  @param[in]  mode  Mode to configure the QSPI into. One of @ref DL_QSPI_MODE.
 *
 *  @sa         DL_QSPI_init
 */
__STATIC_INLINE void DL_QSPI_setMode(QSPI_Regs *qspi, DL_QSPI_MODE mode)
{
    DL_Common_updateReg(&qspi->CTL1, QSPI_CTL1_CP_ENABLE, QSPI_CTL1_CP_MASK);
}

/**
 *  @brief      Get the current mode for the QSPI (controller/peripheral)
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The currently configured mode for the QSPI (controller/peripheral)
 *
 *  @retval     One of @ref DL_QSPI_MODE.
 */
__STATIC_INLINE DL_QSPI_MODE DL_QSPI_getMode(QSPI_Regs *qspi)
{
    uint32_t mode = qspi->CTL1 & QSPI_CTL1_CP_MASK;

    return (DL_QSPI_MODE)(mode);
}

/**
 *  @brief      Set the bit order used for transfers
 *
 *  @param[in]  qspi       Pointer to the register overlay for the peripheral
 *  @param[in]  bitOrder  Order for bits to be sent out during transfer. One of
 *                        @ref DL_QSPI_BIT_ORDER.
 *
 *  @sa         DL_QSPI_init
 */
__STATIC_INLINE void DL_QSPI_setBitOrder(
    QSPI_Regs *qspi, DL_QSPI_BIT_ORDER bitOrder)
{
    DL_Common_updateReg(&qspi->CTL1, (uint32_t) bitOrder, QSPI_CTL1_MSB_MASK);
}

/**
 *  @brief      Get the current bit order used for transfers
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The currently configured bit order
 *
 *  @retval     One of @ref DL_QSPI_BIT_ORDER.
 */
__STATIC_INLINE DL_QSPI_BIT_ORDER DL_QSPI_getBitOrder(QSPI_Regs *qspi)
{
    uint32_t bitOrder = qspi->CTL1 & QSPI_CTL1_MSB_MASK;

    return (DL_QSPI_BIT_ORDER)(bitOrder);
}

/**
 *  @brief      Enables loopback mode
 *
 *  Enables the loopback mode. When enabled, the output of the transmit serial
 *  shifter is connected to the input of the receive serial shifter internally.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_enableLoopbackMode(QSPI_Regs *qspi)
{
    qspi->CTL1 |= QSPI_CTL1_LBM_ENABLE;
}

/**
 *  @brief      Disables loopback mode
 *
 *  Disables the loopback mode. When disabled, the transmit serial shifter and
 *  receive serial shifter are not connected internally.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_disableLoopbackMode(QSPI_Regs *qspi)
{
    qspi->CTL1 &= ~(QSPI_CTL1_LBM_MASK);
}

/**
 *  @brief      Checks if the loopback mode is enabled
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     The status of the loopback mode
 *
 *  @retval     true if loopback mode is enabled
 *  @retval     false if loopback mode is disabled
 */
__STATIC_INLINE bool DL_QSPI_isLoopbackModeEnabled(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_LBM_MASK) == QSPI_CTL1_LBM_ENABLE);
}

/**
 *  @brief      Set counter for repeated transmit
 *
 *  Repeated transmit allows you to send the same data multiple times. This is
 *  essentially the same as writing the data into the transmit buffer
 *  repeatedly.
 *
 *  @param[in]  qspi         pointer to the register overlay for the peripheral
 *  @param[in]  numRepeats  number of times to repeat the transfer. Should be a
 *                          value between 0-255.
 *                          @arg 0 disables the repeated transfer
 *                          @arg 1-255 repeats that many times. So will be sent
 *                               numRepeats + 1 times in total.
 */
__STATIC_INLINE void DL_QSPI_setRepeatTransmit(
    QSPI_Regs *qspi, uint32_t numRepeats)
{
    DL_Common_updateReg(&qspi->CTL1, numRepeats << QSPI_CTL1_REPEATTX_OFS,
        QSPI_CTL1_REPEATTX_MASK);
}

/**
 *  @brief      Get counter for repeated transmit
 *
 *  Repeated transmit allows you to send the same data multiple times. This is
 *  essentially the same as writing the data into the transmit buffer
 *  repeatedly.
 *
 *  @param[in]  qspi    pointer to the register overlay for the peripheral
 *
 *  @return     Number of repeats used for transfer
 *
 *  @retval     0      Repeated transfer is disabled
 *  @retval     1-255  Repeat that many times.
 */
__STATIC_INLINE uint32_t DL_QSPI_getRepeatTransmit(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_REPEATTX_MASK) >> QSPI_CTL1_REPEATTX_OFS);
}

/**
 *  @brief      Enables data alignment on chip select for peripherals
 *
 *  When enabled, the receieve bit counter is cleared automatically when the
 *  chip select gets set inactive. This helps the peripheral synchronize again to
 *  the controller in case of a disturbance or glitch on the clock line or during
 *  initialization.
 *
 *  This is only relevant when in peripheral mode.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_enablePeripheralAlignDataOnChipSelect(
    QSPI_Regs *qspi)
{
    qspi->CTL0 |= QSPI_CTL0_CSCLR_ENABLE;
}

/**
 *  @brief      Disables data alignment on chip select for peripherals
 *
 *  When disable, the receieve bit counter is not cleared automatically when
 *  the chip select gets set inactive.
 *
 *  This is only relevant when in peripheral mode.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_disablePeripheralAlignDataOnChipSelect(
    QSPI_Regs *qspi)
{
    qspi->CTL0 &= ~(QSPI_CTL0_CSCLR_MASK);
}

/**
 *  @brief      Checks if data alignment on chip select for peripherals is enabled
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     The status of data alignment on chip select
 *
 *  @retval     true   Data alignment on chip select is enabled
 *  @retval     false  Data alignment on chip select is disabled
 */
__STATIC_INLINE bool DL_QSPI_isPeripheralAlignDataOnChipSelectEnabled(
    QSPI_Regs *qspi)
{
    return ((qspi->CTL0 & QSPI_CTL0_CSCLR_MASK) == QSPI_CTL0_CSCLR_ENABLE);
}

/**
 *  @brief      Enables packing feature
 *
 *  When enabled, two entries of the RX FIFO are returned as a 32-bit value.
 *  When reading the TX FIFO, if the last write to that field was a 32-bit
 *  write, those 32-bits will be returned.
 *  When writing to the TX FIFO, a 32-bit write will be written as one FIFO
 *  entry.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_enablePacking(QSPI_Regs *qspi)
{
    qspi->CTL0 |= QSPI_CTL0_PACKEN_ENABLED;
}

/**
 *  @brief      Disables packing feature
 *
 *  When disabled, 1 entry of the RX FIFO is returned as a 16-bit value.
 *  When reading the TX FIFO, if the last write to that field was a 16-bit
 *  write, those 16-bits wil be returned.
 *  When writing to the TX FIFO, a 32-bit write will be written as two FIFO
 *  entries.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_disablePacking(QSPI_Regs *qspi)
{
    qspi->CTL0 &= ~(QSPI_CTL0_PACKEN_MASK);
}

/**
 *  @brief      Checks if packing feature is enabled
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     If packing is enabled
 *
 *  @retval     true   Packing is enabled
 *  @retval     false  Packing is disabled
 */
__STATIC_INLINE bool DL_QSPI_isPackingEnabled(QSPI_Regs *qspi)
{
    return ((qspi->CTL0 & QSPI_CTL0_PACKEN_MASK) == QSPI_CTL0_PACKEN_ENABLED);
}

/**
 *  @brief      Set chip select used for controller or peripheral mode
 *
 *  Choose which chip select should be used for data transfer. User must ensure
 *  one of the 4-wire frame formats were selected using @ref DL_QSPI_init or
 *  @ref DL_QSPI_setFrameFormat.
 *
 * This API is for both controller and peripheral modes, and the chip select
 * can be changed in the application by calling this API.
 *
 *  @param[in]  qspi         pointer to the register overlay for the peripheral
 *  @param[in]  chipSelect  the chip select pin to use. One of @ref
 *                          DL_QSPI_CHIP_SELECT.
 */
__STATIC_INLINE void DL_QSPI_setChipSelect(
    QSPI_Regs *qspi, DL_QSPI_CHIP_SELECT chipSelect)
{
    DL_Common_updateReg(
        &qspi->CTL0, (uint32_t) chipSelect, QSPI_CTL0_CSSEL_MASK);
}

/**
 *  @brief      Get chip select used for controller or peripheral mode
 *
 *  This API is for both controller and peripheral modes.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     The current chip selected used for data transfer
 *
 *  @retval     One of @ref DL_QSPI_CHIP_SELECT.
 */
__STATIC_INLINE DL_QSPI_CHIP_SELECT DL_QSPI_getChipSelect(QSPI_Regs *qspi)
{
    uint32_t chipSelect = qspi->CTL0 & QSPI_CTL0_CSSEL_MASK;

    return (DL_QSPI_CHIP_SELECT)(chipSelect);
}

/**
 *  @brief      Set peripheral receive timeout
 *
 *  Sets the number of clock cycles before a receive timeout occurs.
 *
 *  This is only relevant when in peripheral mode.
 *
 *  @param[in]  qspi      pointer to the register overlay for the peripheral
 *  @param[in]  timeout  Number of clock cycles before a receive timeout
 *                       occurs. Must be between 0-63.
 *                       @arg 0 Disables receive timeout
 *                       @arg 1-63 Number of clock cycles before timeout
 */
__STATIC_INLINE void DL_QSPI_setPeripheralReceiveTimeout(
    QSPI_Regs *qspi, uint32_t timeout)
{
    DL_Common_updateReg(&qspi->CTL1, timeout << QSPI_CTL1_RXTIMEOUT_OFS,
        QSPI_CTL1_RXTIMEOUT_MASK);
}

/**
 *  @brief      Get peripheral receive timeout
 *
 *  This is only relevant when in peripheral mode.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *
 *  @return     The current amount of cycles used for a peripheral timeout
 *
 *  @retval     0     indicating that receive timeout is disabled
 *  @retval     1-63  number of clock cycles before a receive timeout occurs
 */
__STATIC_INLINE uint32_t DL_QSPI_getPeripheralReceiveTimeout(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_RXTIMEOUT_MASK) >> QSPI_CTL1_RXTIMEOUT_OFS);
}

/**
 *  @brief      Configure the command/data mode
 *
 *  Command/data mode allows the hardware to control the C/D pin to indicate
 *  whether the data being transferred is a command or just data. The C/D pin
 *  is low for commands and high for data.
 *
 *  You can use this API to manually set the C/D pin by passing in @ref
 *  DL_QSPI_CD_MODE_DATA or @ref DL_QSPI_CD_MODE_COMMAND. You can also have the
 *  C/D pin switch automatically by passing in how many bytes are part of the
 *  command transfer. The C/D pin will stay low until that many bytes are sent
 *  out and then automatically switch to high.
 *
 *  This is only relevant when in controller mode.
 *
 *  @param[in]  qspi    pointer to the register overlay for the peripheral
 *  @param[in]  config configuration for command/data mode. One of:
 *                     @arg DL_QSPI_CD_MODE_DATA
 *                     @arg DL_QSPI_CD_MODE_COMMAND
 *                     @arg A value between 1-14 to indicate how many bytes
 *                          should be sent as command
 */
__STATIC_INLINE void DL_QSPI_setControllerCommandDataModeConfig(
    QSPI_Regs *qspi, uint32_t config)
{
    DL_Common_updateReg(
        &qspi->CTL1, config << QSPI_CTL1_CDMODE_OFS, QSPI_CTL1_CDMODE_MASK);
}

/**
 *  @brief      Get the command/data mode configuration
 *
 *  Returns the current configuration for command/data mode. When using the
 *  automatic C/D pin the value read back will be the number of command bytes
 *  remaining to be sent.
 *
 *  @param[in]  qspi    pointer to the register overlay for the peripheral
 *
 *  @return     The current configuration for command/data mode
 *
 *  @retval     DL_QSPI_CD_MODE_DATA    if in manual data mode
 *  @retval     DL_QSPI_CD_MODE_COMMAND if in manual command mode
 *  @retval     1-14 indicating how many command bytes still need to be sent
 */
__STATIC_INLINE uint32_t DL_QSPI_getControllerCommandDataModeConfig(
    QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_CDMODE_MASK) >> QSPI_CTL1_CDMODE_OFS);
}

/**
 *  @brief      Enables command/data mode
 *
 *  When command/data mode is enabled, the C/D pin is used to differentiate
 *  between command and data during the transaction.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @sa         DL_QSPI_setControllerCommandDataModeConfig
 */
__STATIC_INLINE void DL_QSPI_enableControllerCommandDataMode(QSPI_Regs *qspi)
{
    qspi->CTL1 |= QSPI_CTL1_CDENABLE_ENABLE;
}

/**
 *  @brief      Disables command/data mode
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_disableControllerCommandDataMode(QSPI_Regs *qspi)
{
    qspi->CTL1 &= ~(QSPI_CTL1_CDENABLE_MASK);
}

/**
 *  @brief      Checks if command/data mode is enabled
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     The status of command/data mode
 *
 *  @retval     true  Command/data mode is enabled
 *  @retval     false Command/data mode is disabled
 */
__STATIC_INLINE bool DL_QSPI_isControllerCommandDataModeEnabled(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_CDENABLE_MASK) == QSPI_CTL1_CDENABLE_ENABLE);
}

/**
 *  @brief      Enables peripheral data output
 *
 *  When peripheral data output is enabled, the peripheral can drive the POCI output pin.
 *  This will cause problems if all peripherals have their RXD lines tied together
 *  and the controller is trying to broadcast a message to all peripherals while
 *  ensuring only one peripheral drives data onto its serial output line.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_enablePeripheralDataOutput(QSPI_Regs *qspi)
{
    qspi->CTL1 &= ~(QSPI_CTL1_POD_MASK);
}

/**
 *  @brief      Disables peripheral data output
 *
 *  When peripheral data output is disabled, the peripheral cannot drive the POCI output
 *  pin. This allows multiple peripherals that have their RXD lines tied together to
 *  receive a broadcasted message from a controller.
 *
 *  This is only relevant when in peripheral mode.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 */
__STATIC_INLINE void DL_QSPI_disablePeripheralDataOutput(QSPI_Regs *qspi)
{
    qspi->CTL1 |= QSPI_CTL1_POD_ENABLE;
}

/**
 *  @brief      Checks if peripheral data output is enabled
 *
 *  This is only relevant when in peripheral mode.
 *
 *  @param[in]  qspi  pointer to the register overlay for the peripheral
 *
 *  @return     If peripheral data output is enabled
 *
 *  @retval     true if peripheral data output is enabled
 *  @retval     false if peripheral data output is disabled
 */
__STATIC_INLINE bool DL_QSPI_isPeripheralDataOutputEnabled(QSPI_Regs *qspi)
{
    return ((qspi->CTL1 & QSPI_CTL1_POD_MASK) == QSPI_CTL1_POD_DISABLE);
}

/**
 *  @brief      Set the delay sampling
 *
 *  In controller mode only, the data on the input pin will be delayed sampled
 *  by the defined QSPI clock cycles. The delay can be adjusted in steps of QSPI
 *  input clock steps. The maximum allowed delay should not exceed the length
 *  of one data frame
 *
 *  @param[in]  qspi     Pointer to the register overlay for the peripheral
 *  @param[in]  delay   The number of QSPI clock cycles to delay sampling on
 *                      input pin. Value between 0-15.
 */
__STATIC_INLINE void DL_QSPI_setDelayedSampling(QSPI_Regs *qspi, uint32_t delay)
{
    DL_Common_updateReg(&qspi->CLKCTL, delay << QSPI_CLKCTL_DSAMPLE_OFS,
        QSPI_CLKCTL_DSAMPLE_MASK);
}

/**
 *  @brief      Get the delay sampling
 *
 *  In controller mode only, the data on the input pin will be delayed sampled
 *  by the defined QSPI clock cycles. The delay can be adjusted in steps of QSPI
 *  input clock steps. The maximum allowed delay should not exceed the length
 *  of one data frame
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The amount of delay sampling on the input pin in QSPI
 *              clock cycles.
 *
 *  @retval     0 - 15. The amount of delay sampling in QSPI clock cycles.
 */
__STATIC_INLINE uint32_t DL_QSPI_getDelayedSampling(QSPI_Regs *qspi)
{
    return (qspi->CLKCTL & QSPI_CLKCTL_DSAMPLE_MASK >> QSPI_CLKCTL_DSAMPLE_OFS);
}

/**
 *  @brief      Set the RX and TX FIFO interrupt threshold level
 *
 *  Select the threshold for the receive and transmit FIFO interrupts. The
 *  interrupts are generated based on a transition through a level rather
 *  than being based on the level. That is, the interrupts are generated when
 *  the fill level progresses through the trigger level.
 *
 *  For example, if the trigger level is set to the half-way mark, the interrupt
 *  is triggered when the FIFO becomes half empty/full.
 *
 *  Out of reset, the FIFOs are triggered to interrupt at half-way mark.
 *
 *  @param[in]  qspi             Pointer to the register overlay for the peripheral
 *  @param[in]  rxThreshold     One of @ref DL_QSPI_RX_FIFO_LEVEL
 *  @param[in]  txThreshold     One of @ref DL_QSPI_TX_FIFO_LEVEL
 *
 */
__STATIC_INLINE void DL_QSPI_setFIFOThreshold(QSPI_Regs *qspi,
    DL_QSPI_RX_FIFO_LEVEL rxThreshold, DL_QSPI_TX_FIFO_LEVEL txThreshold)
{
    DL_Common_updateReg(&qspi->IFLS,
        (uint32_t) rxThreshold | (uint32_t) txThreshold,
        QSPI_IFLS_RXIFLSEL_MASK | QSPI_IFLS_TXIFLSEL_MASK);
}

/**
 *  @brief      Get the TX FIFO interrupt threshold level
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The TX FIFO interrupt threshold level
 *
 *  @retval     One of @ref DL_QSPI_TX_FIFO_LEVEL
 */
__STATIC_INLINE DL_QSPI_TX_FIFO_LEVEL DL_QSPI_getTXFIFOThreshold(QSPI_Regs *qspi)
{
    uint32_t txThreshold = qspi->IFLS & QSPI_IFLS_TXIFLSEL_MASK;

    return (DL_QSPI_TX_FIFO_LEVEL)(txThreshold);
}

/**
 *  @brief      Get the RX FIFO interrupt threshold level
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The RX FIFO interrupt threshold level
 *
 *  @retval     One of @ref DL_QSPI_RX_FIFO_LEVEL
 */
__STATIC_INLINE DL_QSPI_RX_FIFO_LEVEL DL_QSPI_getRXFIFOThreshold(QSPI_Regs *qspi)
{
    uint32_t rxThreshold = qspi->IFLS & QSPI_IFLS_RXIFLSEL_MASK;

    return (DL_QSPI_RX_FIFO_LEVEL)(rxThreshold);
}

/**
 *  @brief      Set the QSPI bit rate serial clock divider (SCR)
 *
 * The QSPI includes a programmable bit rate clock divider and prescaler to
 * generate the serial output clock. The bit rates are supported up to
 * FUNCCLK/2. The functional clock selection depends on the specific device,
 * please refer to the device datasheet and PMU/Clock section.
 *
 * The QSPI output bit rate is calculated with the following formula:
 * QSPI bit rate = (QSPI functional clock) / ((1 + SCR)*2)
 * Given this formula, the SCR can be calculated:
 * SCR = (QSPI functional clock) / ((2 * QSPI bit rate) - 1)
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *  @param[in]  SCR  The QSPI serial clock divider. Value between 0-1023.
 */
__STATIC_INLINE void DL_QSPI_setBitRateSerialClockDivider(
    QSPI_Regs *qspi, uint32_t SCR)
{
    DL_Common_updateReg(&qspi->CLKCTL, SCR, QSPI_CLKCTL_SCR_MASK);
}

/**
 *  @brief      Get the QSPI bit rate serial clock divider (SCR)
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @return     The current bit rate serial clock divider
 *
 *  @retval     The QSPI SCR. Value from 0-1023
 */
__STATIC_INLINE uint32_t DL_QSPI_getBitRateSerialClockDivider(QSPI_Regs *qspi)
{
    return (qspi->CLKCTL & QSPI_CLKCTL_SCR_MASK);
}

/**
 *  @brief      Writes 8-bit data into the TX FIFO for transmit
 *
 *  Puts the data into the TX FIFO without checking its status. Use if already
 *  sure the TX FIFO has space for the write. See related APIs for additional
 *  transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 8 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @sa         DL_QSPI_transmitDataBlocking8
 *  @sa         DL_QSPI_transmitDataCheck8
 */
__STATIC_INLINE void DL_QSPI_transmitData8(QSPI_Regs *qspi, uint8_t data)
{
    qspi->TXDATA = data;
}

/**
 *  @brief      Writes 16-bit data into the TX FIFO for transmit
 *
 *  Puts the data into the TX FIFO without checking its status. Use if already
 *  sure the TX FIFO has space for the write. See related APIs for additional
 *  transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 16 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @sa         DL_QSPI_transmitDataBlocking16
 *  @sa         DL_QSPI_transmitDataCheck16
 */
__STATIC_INLINE void DL_QSPI_transmitData16(QSPI_Regs *qspi, uint16_t data)
{
    qspi->TXDATA = data;
}

/**
 *  @brief      Writes 32-bit data into the TX FIFO for transmit
 *
 *  Puts the data into the TX FIFO without checking its status. Use if already
 *  sure the TX FIFO has space for the write. See related APIs for additional
 *  transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 32 bits.
 *
 *  NOTE: If packing is enabled by calling @ref DL_QSPI_enablePacking prior to
 *  calling this API, then a 32-bit write will be written as one FIFO entry. If
 *  packing is disabled, then a 32-bit write will be written as two FIFO
 *  entries.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @sa         DL_QSPI_enablePacking
 *  @sa         DL_QSPI_transmitDataBlocking32
 *  @sa         DL_QSPI_transmitDataCheck32
 */
__STATIC_INLINE void DL_QSPI_transmitData32(QSPI_Regs *qspi, uint32_t data)
{
    qspi->TXDATA = data;
}

/**
 *  @brief      Reads 8-bit data from the RX FIFO
 *
 *  Reads the data from the RX FIFO without checking its status. Use if
 *  already sure the RX FIFO has data available. See related APIs for
 *  additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 8 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *
 *  @return     The data in the RX FIFO
 *
 *  @sa         DL_QSPI_receiveDataBlocking8
 *  @sa         DL_QSPI_receiveDataCheck8
 */
__STATIC_INLINE uint8_t DL_QSPI_receiveData8(QSPI_Regs *qspi)
{
    return ((uint8_t)(qspi->RXDATA));
}

/**
 *  @brief      Reads 16-bit data from the RX FIFO
 *
 *  Reads the data from the RX FIFO without checking its status. Use if
 *  already sure the RX FIFO has data available. See related APIs for
 *  additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 16 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *
 *  @return     The data in the RX FIFO
 *
 *  @sa         DL_QSPI_receiveDataBlocking16
 *  @sa         DL_QSPI_receiveDataCheck16
 */
__STATIC_INLINE uint16_t DL_QSPI_receiveData16(QSPI_Regs *qspi)
{
    return ((uint16_t)(qspi->RXDATA));
}

/**
 *  @brief      Reads 32-bit data from the RX FIFO
 *
 *  Reads the data from the RX FIFO without checking its status. Use if
 *  already sure the RX FIFO has data available. See related APIs for
 *  additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 32 bits.
 *
 *  NOTE: Requires that packing has been enabled by calling
 *  @ref DL_QSPI_enablePacking prior to calling this API. When packing is
 *  enabled, two entries of the RX FIFO are returned as a 32-bit value.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *
 *  @return     The data in the RX FIFO
 *
 *  @pre         DL_QSPI_enablePacking
 *
 *  @sa         DL_QSPI_receiveDataBlocking32
 *  @sa         DL_QSPI_receiveDataCheck32
 */
__STATIC_INLINE uint32_t DL_QSPI_receiveData32(QSPI_Regs *qspi)
{
    return qspi->RXDATA;
}

/**
 *  @brief      Enable QSPI interrupts
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to enable. Bitwise OR of
 *                             @ref DL_QSPI_INTERRUPT.
 */
__STATIC_INLINE void DL_QSPI_enableInterrupt(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    qspi->CPU_INT.IMASK |= interruptMask;
}

/**
 *  @brief      Disable QSPI interrupts
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to disable. Bitwise OR of
 *                             @ref DL_QSPI_INTERRUPT.
 */
__STATIC_INLINE void DL_QSPI_disableInterrupt(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    qspi->CPU_INT.IMASK &= ~(interruptMask);
}

/**
 *  @brief      Check which QSPI interrupts are enabled
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to check. Bitwise OR of
 *                             @ref DL_QSPI_INTERRUPT.
 *
 *  @return     Which of the requested QSPI interrupts are enabled
 *
 *  @retval     Bitwise OR of @ref DL_QSPI_INTERRUPT values
 */
__STATIC_INLINE uint32_t DL_QSPI_getEnabledInterrupts(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    return (qspi->CPU_INT.IMASK & interruptMask);
}

/**
 *  @brief      Check interrupt flag of enabled QSPI interrupts
 *
 *  Checks if any of the QSPI interrupts that were previously enabled are
 *  pending.
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to check. Bitwise OR of
 *                             @ref DL_QSPI_INTERRUPT.
 *
 *  @return     Which of the requested QSPI interrupts are pending
 *
 *  @retval     Bitwise OR of @ref DL_QSPI_INTERRUPT values
 *
 *  @sa         DL_QSPI_enableInterrupt
 */
__STATIC_INLINE uint32_t DL_QSPI_getEnabledInterruptStatus(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    return (qspi->CPU_INT.MIS & interruptMask);
}

/**
 *  @brief      Check interrupt flag of any QSPI interrupt
 *
 *  Checks if any of the QSPI interrupts are pending. Interrupts do not have to
 *  be previously enabled.
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to check. Bitwise OR of
 *                             @ref DL_QSPI_INTERRUPT.
 *
 *  @return     Which of the requested QSPI interrupts are pending
 *
 *  @retval     Bitwise OR of @ref DL_QSPI_INTERRUPT values
 */
__STATIC_INLINE uint32_t DL_QSPI_getRawInterruptStatus(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    return (qspi->CPU_INT.RIS & interruptMask);
}

/**
 *  @brief      Get highest priority pending QSPI interrupt
 *
 *  Checks if any of the QSPI interrupts are pending. Interrupts do not have to
 *  be previously enabled.
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *
 *  @return     The highest priority pending QSPI interrupt. One of @ref
 *               DL_QSPI_IIDX
 */
__STATIC_INLINE DL_QSPI_IIDX DL_QSPI_getPendingInterrupt(QSPI_Regs *qspi)
{
    return ((DL_QSPI_IIDX) qspi->CPU_INT.IIDX);
}

/**
 *  @brief      Clear pending QSPI interrupts
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to clear. Bitwise OR of
 *                             @ref DL_QSPI_INTERRUPT.
 */
__STATIC_INLINE void DL_QSPI_clearInterruptStatus(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    qspi->CPU_INT.ICLR = interruptMask;
}

/**
 *  @brief      Blocks to ensure transmit is ready before sending data
 *
 *  Puts the data into the TX FIFO after blocking to ensure the TX FIFO is not
 *  full. Will wait indefinitely until there is space in the TX FIFO. See
 *  related APIs for additional transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 8 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @sa         DL_QSPI_transmitData8
 *  @sa         DL_QSPI_transmitDataCheck8
 */
void DL_QSPI_transmitDataBlocking8(QSPI_Regs *qspi, uint8_t data);

/**
 *  @brief      Blocks to ensure transmit is ready before sending data
 *
 *  Puts the data into the TX FIFO after blocking to ensure the TX FIFO is not
 *  full. Will wait indefinitely until there is space in the TX FIFO. See related
 *  APIs for additional transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 16 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @sa         DL_QSPI_transmitData16
 *  @sa         DL_QSPI_transmitDataCheck16
 */
void DL_QSPI_transmitDataBlocking16(QSPI_Regs *qspi, uint16_t data);

/**
 *  @brief      Blocks to ensure transmit is ready before sending data
 *
 *  Puts the data into the TX FIFO after blocking to ensure the TX FIFO is not
 *  full. Will wait indefinitely until there is space in the TX FIFO. See related
 *  APIs for additional transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 32 bits.
 *
 *  NOTE: If packing is enabled by calling @ref DL_QSPI_enablePacking prior to
 *  calling this API, then a 32-bit write will be written as one FIFO entry. If
 *  packing is disabled, then a 32-bit write will be written as two FIFO
 *  entries.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @sa         DL_QSPI_enablePacking
 *  @sa         DL_QSPI_transmitData32
 *  @sa         DL_QSPI_transmitDataCheck32
 */
void DL_QSPI_transmitDataBlocking32(QSPI_Regs *qspi, uint32_t data);

/**
 *  @brief      Blocks to ensure receive is ready before reading data
 *
 *  Reads the data from the RX FIFO after blocking to ensure the RX FIFO is not
 *  empty. Will wait indefinitely until there is data in the RX FIFO. See
 *  related APIs for additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 8 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *
 *  @return     The data in the RX FIFO
 *
 *  @sa         DL_QSPI_transmitData8
 *  @sa         DL_QSPI_transmitDataCheck8
 */
uint8_t DL_QSPI_receiveDataBlocking8(QSPI_Regs *qspi);

/**
 *  @brief      Blocks to ensure receive is ready before reading data
 *
 *  Reads the data from the RX FIFO after blocking to ensure the RX FIFO is not
 *  empty. Will wait indefinitely until there is data in the RX FIFO. See
 *  related APIs for additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 16 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *
 *  @return     The data in the RX FIFO
 *
 *  @sa         DL_QSPI_transmitData16
 *  @sa         DL_QSPI_transmitDataCheck16
 */
uint16_t DL_QSPI_receiveDataBlocking16(QSPI_Regs *qspi);

/**
 *  @brief      Blocks to ensure receive is ready before reading data
 *
 *  Reads the data from the RX FIFO after blocking to ensure the RX FIFO is not
 *  empty. Will wait indefinitely until there is data in the RX FIFO. See
 *  related APIs for additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 32 bits.
 *
 *  NOTE: Requires that packing has been enabled by calling
 *  @ref DL_QSPI_enablePacking prior to calling this API. When packing is
 *  enabled, two entries of the RX FIFO are returned as a 32-bit value.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *
 *  @return     The data in the RX FIFO
 *
 *  @pre         DL_QSPI_enablePacking
 *
 *  @sa         DL_QSPI_transmitData32
 *  @sa         DL_QSPI_transmitDataCheck32
 */
uint32_t DL_QSPI_receiveDataBlocking32(QSPI_Regs *qspi);

/**
 *  @brief      Checks the TX FIFO before trying to transmit data
 *
 *  Checks if the TX FIFO is already full before trying to add new data to the
 *  FIFO. Exits immediately if full rather than trying to block. See related
 *  APIs for additional transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 8 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @return     If the transmit occurred
 *
 *  @retval     true  if data was added to the TX FIFO
 *  @retval     false if the TX FIFO was full and data was not added
 *
 *  @sa         DL_QSPI_transmitData8
 *  @sa         DL_QSPI_transmitDataBlocking8
 */
bool DL_QSPI_transmitDataCheck8(QSPI_Regs *qspi, uint8_t data);

/**
 *  @brief      Checks the TX FIFO before trying to transmit data
 *
 *  Checks if the TX FIFO is already full before trying to add new data to the
 *  FIFO. Exits immediately if full rather than trying to block. See related
 *  APIs for additional transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 16 bits.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @return     If the transmit occurred
 *
 *  @retval     true  if data was added to the TX FIFO
 *  @retval     false if the TX FIFO was full and data was not added
 *
 *  @sa         DL_QSPI_transmitData16
 *  @sa         DL_QSPI_transmitDataBlocking16
 */
bool DL_QSPI_transmitDataCheck16(QSPI_Regs *qspi, uint16_t data);

/**
 *  @brief      Checks the TX FIFO before trying to transmit data
 *
 *  Checks if the TX FIFO is already full before trying to add new data to the
 *  FIFO. Exits immediately if full rather than trying to block. See related
 *  APIs for additional transmit options.
 *
 *  Can be used for any data transfers that are less than or equal to 32 bits.
 *
 *  NOTE: If packing is enabled by calling @ref DL_QSPI_enablePacking prior to
 *  calling this API, then a 32-bit write will be written as one FIFO entry. If
 *  packing is disabled, then a 32-bit write will be written as two FIFO
 *  entries.
 *
 *  @param[in]  qspi   pointer to the register overlay for the peripheral
 *  @param[in]  data  data to send
 *
 *  @return     If the transmit occurred
 *
 *  @retval     true  if data was added to the TX FIFO
 *  @retval     false if the TX FIFO was full and data was not added
 *
 *  @sa         DL_QSPI_enablePacking
 *  @sa         DL_QSPI_transmitData32
 *  @sa         DL_QSPI_transmitDataBlocking32
 */
bool DL_QSPI_transmitDataCheck32(QSPI_Regs *qspi, uint32_t data);

/**
 *  @brief      Checks the RX FIFO before trying to transmit data
 *
 *  Checks if the RX FIFO is already empty before trying to read new data from
 *  the FIFO. Exits immediately if empty rather than trying to block. See
 *  related APIs for additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 8 bits.
 *
 *  @param[in]  qspi    pointer to the register overlay for the peripheral
 *  @param[in]  buffer a buffer to write the received data into
 *
 *  @return     If the receive occurred
 *
 *  @retval     true  if data was read from the RX FIFO
 *  @retval     false if the RX FIFO was empty and data was not read
 *
 *  @sa         DL_QSPI_receiveData8
 *  @sa         DL_QSPI_receiveDataBlocking8
 */
bool DL_QSPI_receiveDataCheck8(QSPI_Regs *qspi, uint8_t *buffer);

/**
 *  @brief      Checks the RX FIFO before trying to transmit data
 *
 *  Checks if the RX FIFO is already empty before trying to read new data from
 *  the FIFO. Exits immediately if empty rather than trying to block. See
 *  related APIs for additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 16 bits.
 *
 *  @param[in]  qspi    pointer to the register overlay for the peripheral
 *  @param[in]  buffer a buffer to write the received data into
 *
 *  @return     If the receive occurred
 *
 *  @retval     true  if data was read from the RX FIFO
 *  @retval     false if the RX FIFO was empty and data was not read
 *
 *  @sa         DL_QSPI_receiveData16
 *  @sa         DL_QSPI_receiveDataBlocking16
 */
bool DL_QSPI_receiveDataCheck16(QSPI_Regs *qspi, uint16_t *buffer);

/**
 *  @brief      Checks the RX FIFO before trying to transmit data
 *
 *  Checks if the RX FIFO is already empty before trying to read new data from
 *  the FIFO. Exits immediately if empty rather than trying to block. See
 *  related APIs for additional receive options.
 *
 *  Can be used for any data transfers that are less than or equal to 32 bits.
 *
 *  NOTE: Requires that packing has been enabled by calling
 *  @ref DL_QSPI_enablePacking prior to calling this API. When packing is
 *  enabled, two entries of the RX FIFO are returned as a 32-bit value.
 *
 *  @param[in]  qspi    pointer to the register overlay for the peripheral
 *  @param[in]  buffer a buffer to write the received data into
 *
 *  @return     If the receive occurred
 *
 *  @retval     true  if data was read from the RX FIFO
 *  @retval     false if the RX FIFO was empty and data was not read
 *
 *  @pre         DL_QSPI_enablePacking
 *
 *  @sa         DL_QSPI_receiveData32
 *  @sa         DL_QSPI_receiveDataBlocking32
 */
bool DL_QSPI_receiveDataCheck32(QSPI_Regs *qspi, uint32_t *buffer);

/**
 *  @brief       Read all available data out of the RX FIFO using 8 bit access
 *
 *  @param[in]   qspi       Pointer to the register overlay for the peripheral
 *  @param[out]  buffer    Buffer to write received data into
 *  @param[in]   maxCount  Max number of bytes to read from the RX FIFO
 *
 *  @return      Number of bytes read from the RX FIFO
 */
uint32_t DL_QSPI_drainRXFIFO8(
    QSPI_Regs *qspi, uint8_t *buffer, uint32_t maxCount);

/**
 *  @brief       Read all available data out of the RX FIFO using 16 bit access
 *
 *  @param[in]   qspi       Pointer to the register overlay for the peripheral
 *  @param[out]  buffer    Buffer to write received data into
 *  @param[in]   maxCount  Max number of halfwords to read from the RX FIFO
 *
 *  @return      Number of halfwords read from the RX FIFO
 */
uint32_t DL_QSPI_drainRXFIFO16(
    QSPI_Regs *qspi, uint16_t *buffer, uint32_t maxCount);

/**
 *  @brief       Read all available data out of the RX FIFO using 32 bit access
 *
 *  NOTE: Requires that packing has been enabled by calling
 *  @ref DL_QSPI_enablePacking prior to calling this API. When packing is
 *  enabled, two entries of the RX FIFO are returned as a 32-bit value.
 *
 *  @param[in]   qspi       Pointer to the register overlay for the peripheral
 *  @param[out]  buffer    Buffer to write received data into
 *  @param[in]   maxCount  Max number of words to read from the RX FIFO
 *
 *  @return      Number of words read from the RX FIFO
 *
 *  @pre         DL_QSPI_enablePacking
 *
 */
uint32_t DL_QSPI_drainRXFIFO32(
    QSPI_Regs *qspi, uint32_t *buffer, uint32_t maxCount);

/**
 *  @brief      Fill the TX FIFO using 8 bit access
 *
 *  Continuously write data into the TX FIFO until it is filled up or count has
 *  been reached.
 *
 *  @param[in]  qspi     Pointer to the register overlay for the peripheral
 *  @param[in]  buffer  Buffer of data to write to the TX FIFO
 *  @param[in]  count   Max number of bytes to write to the TX FIFO
 *
 *  @return     Number of bytes written to the TX FIFO
 */
uint32_t DL_QSPI_fillTXFIFO8(QSPI_Regs *qspi, uint8_t *buffer, uint32_t count);

/**
 *  @brief      Fill the TX FIFO using 16 bit access
 *
 *  Continuously write data into the TX FIFO until it is filled up or count has
 *  been reached.
 *
 *  @param[in]  qspi     Pointer to the register overlay for the peripheral
 *  @param[in]  buffer  Buffer of data to write to the TX FIFO
 *  @param[in]  count   Max number of halfwords to write to the TX FIFO
 *
 *  @return     Number of halfwords written to the TX FIFO
 */
uint32_t DL_QSPI_fillTXFIFO16(QSPI_Regs *qspi, uint16_t *buffer, uint32_t count);

/**
 *  @brief      Fill the TX FIFO using 32 bit access
 *
 *  Continuously write data into the TX FIFO until it is filled up or count has
 *  been reached.
 *
 *  NOTE: If packing is enabled by calling @ref DL_QSPI_enablePacking prior to
 *  calling this API, then a 32-bit write will be written as one FIFO entry. If
 *  packing is disabled, then a 32-bit write will be written as two FIFO
 *  entries.
 *
 *  @param[in]  qspi     Pointer to the register overlay for the peripheral
 *  @param[in]  buffer  Buffer of data to write to the TX FIFO
 *  @param[in]  count   Max number of words to write to the TX FIFO
 *
 *  @return     Number of words written to the TX FIFO
 *
 *  @sa         DL_QSPI_enablePacking
 */
uint32_t DL_QSPI_fillTXFIFO32(QSPI_Regs *qspi, uint32_t *buffer, uint32_t count);

/**
 *  @brief      Enable QSPI interrupt for triggering the DMA receive event
 *
 * Enables the QSPI interrupt to be used as the condition to generate an
 * event to directly trigger the DMA. This API configures the DMA_TRIG_RX
 * register, which is the event publisher used for triggering the DMA to do
 * a receive data transfer.
 *
 * @note Only one interrupt source should be enabled at a time.
 *
 *  @param[in]  qspi       Pointer to the register overlay for the
 *                         peripheral
 *  @param[in]  interrupt  Interrupt to enable as the trigger condition for
 *                         the DMA. One of @ref DL_QSPI_DMA_INTERRUPT_RX.
 */
__STATIC_INLINE void DL_QSPI_enableDMAReceiveEvent(
    QSPI_Regs *qspi, uint32_t interrupt)
{
    qspi->DMA_TRIG_RX.IMASK = interrupt;
}

/**
 *  @brief      Enable QSPI interrupt for triggering the DMA transmit event
 *
 * Enables the QSPI interrupt to be used as the condition to generate an
 * event to directly trigger the DMA. This API configures the DMA_TRIG_TX
 * register, which is the event publisher used for triggering the DMA to do
 * a transmit data transfer.
 *
 * @note DMA_TRIG_TX only has one transmit interrupt source
 *
 *  @param[in]  qspi       Pointer to the register overlay for the
 *                         peripheral
 */
__STATIC_INLINE void DL_QSPI_enableDMATransmitEvent(QSPI_Regs *qspi)
{
    qspi->DMA_TRIG_TX.IMASK = QSPI_DMA_TRIG_TX_IMASK_TX_SET;
}

/**
 *  @brief      Disables QSPI interrupt from triggering the DMA receive event
 *
 * Disables the QSPI interrupt as the condition to generate an event to
 * directly trigger the DMA. This API configures the DMA_TRIG_RX
 * register, which is the event publisher used for triggering the DMA to do
 * a receive data transfer.
 *
 *  @param[in]  qspi       Pointer to the register overlay for the
 *                         peripheral
 *  @param[in]  interrupt  Interrupt to disable as the trigger condition for
 *                         the DMA. One of @ref DL_QSPI_DMA_INTERRUPT_RX.
 */
__STATIC_INLINE void DL_QSPI_disableDMAReceiveEvent(
    QSPI_Regs *qspi, uint32_t interrupt)
{
    qspi->DMA_TRIG_RX.IMASK &= ~(interrupt);
}

/**
 *  @brief      Disables QSPI interrupt from triggering the DMA transmit event
 *
 * Disables the QSPI interrupt as the condition to generate an event to
 * directly trigger the DMA. This API configures the DMA_TRIG_TX
 * register, which is the event publisher used for triggering the DMA to do
 * a transmit data transfer.
 *
 * @note DMA_TRIG_TX only has one transmit interrupt source
 *
 * @param[in]  qspi       Pointer to the register overlay for the
 *                       peripheral
 */
__STATIC_INLINE void DL_QSPI_disableDMATransmitEvent(QSPI_Regs *qspi)
{
    qspi->DMA_TRIG_TX.IMASK = QSPI_DMA_TRIG_TX_IMASK_TX_CLR;
}

/**
 *  @brief      Check which QSPI interrupt for DMA receive events is enabled
 *
 *  This API checks the DMA_TRIG_RX register, which is the event publisher used
 *  for triggering the DMA to do a receive data transfer.
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to check. Bitwise OR of
 *                             @ref DL_QSPI_DMA_INTERRUPT_RX.
 *
 *  @note Only one interrupt source should be enabled at a time.
 *
 *  @return     Which of the requested QSPI interrupts is enabled
 *
 *  @retval     One of @ref DL_QSPI_DMA_INTERRUPT_RX
 */
__STATIC_INLINE uint32_t DL_QSPI_getEnabledDMAReceiveEvent(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    return (qspi->DMA_TRIG_RX.IMASK & interruptMask);
}

/**
 *  @brief      Check if QSPI interrupt for DMA transmit event is enabled
 *
 *  This API checks the DMA_TRIG_TX register, which is the event publisher used
 *  for triggering the DMA to do a transmit data transfer.
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 *
 *  @return     The requested QSPI interrupt status
 *
 *  @retval     DL_QSPI_DMA_INTERRUPT_TX if enabled, 0 if not enabled
 */
__STATIC_INLINE uint32_t DL_QSPI_getEnabledDMATransmitEvent(QSPI_Regs *qspi)
{
    return (qspi->DMA_TRIG_TX.IMASK & QSPI_DMA_TRIG_TX_IMASK_TX_MASK);
}

/**
 *  @brief      Check interrupt flag of enabled QSPI interrupt for DMA receive event
 *
 * Checks if any of the QSPI interrupts for the DMA receive event that were
 * previously enabled are pending.
 * This API checks the DMA_TRIG_RX register, which is the event publisher used
 * for triggering the DMA to do a receive data transfer.
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to check. Bitwise OR of
 *                             @ref DL_QSPI_DMA_INTERRUPT_RX.
 *
 *  @note Only one interrupt source should be enabled at a time.
 *
 *  @return     The requested QSPI interrupt status
 *
 *  @retval     One of @ref DL_QSPI_DMA_INTERRUPT_RX
 *
 *  @sa         DL_QSPI_enableDMAReceiveEvent
 */
__STATIC_INLINE uint32_t DL_QSPI_getEnabledDMAReceiveEventStatus(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    return (qspi->DMA_TRIG_RX.MIS & interruptMask);
}

/**
 *  @brief      Check interrupt flag of enabled QSPI interrupt for DMA transmit event
 *
 * Checks if the QSPI interrupt for the DMA transmit event that was
 * previously enabled is pending.
 * This API checks the DMA_TRIG_TX register, which is the event publisher used
 * for triggering the DMA to do a transmit data transfer.
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 *
 *  @return     The requested QSPI interrupt status
 *
 *  @retval     DL_QSPI_DMA_INTERRUPT_TX if enabled, 0 if not enabled
 *
 *  @sa         DL_QSPI_enableDMATransmitEvent
 */
__STATIC_INLINE uint32_t DL_QSPI_getEnabledDMATransmitEventStatus(QSPI_Regs *qspi)
{
    return (qspi->DMA_TRIG_TX.MIS & QSPI_DMA_TRIG_TX_MIS_TX_MASK);
}

/**
 *  @brief      Check interrupt flag of any QSPI interrupt for DMA receive event
 *
 *  Checks if any of the QSPI interrupts for DMA receive event are pending.
 *  Interrupts do not have to be previously enabled.
 *  This API checks the DMA_TRIG_RX register, which is the event publisher used
 *  for triggering the DMA to do a receive data transfer.
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to check. Bitwise OR of
 *                             @ref DL_QSPI_DMA_INTERRUPT_RX.
 *
 *  @return     Which of the requested QSPI interrupts are pending
 *
 *  @retval     Bitwise OR of @ref DL_QSPI_DMA_INTERRUPT_RX values
 */
__STATIC_INLINE uint32_t DL_QSPI_getRawDMAReceiveEventStatus(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    return (qspi->DMA_TRIG_RX.RIS & interruptMask);
}

/**
 *  @brief      Check interrupt flag of any QSPI interrupt for DMA transmit event
 *
 *  Checks if any of the QSPI interrupts for DMA transmit event are pending.
 *  Interrupts do not have to be previously enabled.
 *  This API checks the DMA_TRIG_TX register, which is the event publisher used
 *  for triggering the DMA to do a transmit data transfer.
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 *
 *  @return     The requested QSPI interrupt status
 *
 *  @retval     DL_QSPI_DMA_INTERRUPT_TX if enabled, 0 if not enabled
 */
__STATIC_INLINE uint32_t DL_QSPI_getRawDMATransmitEventStatus(QSPI_Regs *qspi)
{
    return (qspi->DMA_TRIG_TX.RIS & QSPI_DMA_TRIG_TX_RIS_TX_MASK);
}

/**
 *  @brief      Get highest priority pending QSPI interrupt for DMA receive event
 *
 *  Checks if any of the QSPI interrupts for DMA receive event are pending.
 *  Interrupts do not have to be previously enabled.
 *  This API checks the DMA_TRIG_RX register, which is the event publisher used
 *  for triggering the DMA to do a receive data transfer.
 *
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 *
 *  @return     The highest priority pending QSPI interrupt
 *
 *  @retval     One of @ref DL_QSPI_DMA_IIDX_RX
 */
__STATIC_INLINE DL_QSPI_DMA_IIDX_RX DL_QSPI_getPendingDMAReceiveEvent(
    QSPI_Regs *qspi)
{
    return (DL_QSPI_DMA_IIDX_RX)(qspi->DMA_TRIG_RX.IIDX);
}

/**
 *  @brief      Get highest priority pending QSPI interrupt for DMA transmit event
 *
 *  Checks if the QSPI interrupt for DMA transmit event is pending.
 *  Interrupts do not have to be previously enabled.
 *  This API checks the DMA_TRIG_TX register, which is the event publisher used
 *  for triggering the DMA to do a transmit data transfer.
 *
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 *
 *  @return     The highest priority pending QSPI interrupt
 *
 *  @retval     DL_QSPI_DMA_IIDX_TX if pending, 0 if not pending
 */
__STATIC_INLINE DL_QSPI_DMA_IIDX_TX DL_QSPI_getPendingDMATransmitEvent(
    QSPI_Regs *qspi)
{
    return (DL_QSPI_DMA_IIDX_TX)(qspi->DMA_TRIG_TX.IIDX);
}

/**
 *  @brief      Clear pending QSPI interrupts for DMA receive event
 *
 *  This API checks the DMA_TRIG_RX register, which is the event publisher used
 *  for triggering the DMA to do a receive data transfer.
 *
 *  @param[in]  qspi            Pointer to the register overlay for the
 *                             peripheral
 *  @param[in]  interruptMask  Bit mask of interrupts to clear. Bitwise OR of
 *                             @ref DL_QSPI_DMA_INTERRUPT_RX.
 */
__STATIC_INLINE void DL_QSPI_clearDMAReceiveEventStatus(
    QSPI_Regs *qspi, uint32_t interruptMask)
{
    qspi->DMA_TRIG_RX.ICLR = interruptMask;
}

/**
 *  @brief      Clear pending QSPI interrupt for DMA transmit event
 *
 *  This API checks the DMA_TRIG_TX register, which is the event publisher used
 *  for triggering the DMA to do a transmit data transfer.
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 * @note DMA_TRIG_TX only has one transmit interrupt source
 */
__STATIC_INLINE void DL_QSPI_clearDMATransmitEventStatus(QSPI_Regs *qspi)
{
    qspi->DMA_TRIG_TX.ICLR = QSPI_DMA_TRIG_TX_ICLR_TX_CLR;
}

/**
 *  @brief      Save QSPI configuration before entering a power loss state.
 *
 *  Some MSPM33 peripherals residing in PD1 domain do not retain register
 *  contents when entering STOP or STANDBY modes. Please refer to the datasheet
 *  for the full list of peripheral instances that exhibit this behavior.
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @param[in]  ptr  Configuration backup setup structure. See
 *                  @ref DL_QSPI_backupConfig.
 *
 *  @retval     FALSE if a configuration already exists in ptr (will not be
 *              overwritten). TRUE if a configuration was successfully saved
 *
 *  @sa         DL_QSPI_restoreConfiguration
 */
bool DL_QSPI_saveConfiguration(QSPI_Regs *qspi, DL_QSPI_backupConfig *ptr);

/**
 *  @brief      Restore QSPI configuration after leaving a power loss state.
 *
 *  Some MSPM33 peripherals residing in PD1 domain do not retain register
 *  contents when entering STOP or STANDBY modes. Please refer to the datasheet
 *  for the full list of peripheral instances that exhibit this behavior.
 *
 *  @param[in]  qspi  Pointer to the register overlay for the peripheral
 *
 *  @param[in]  ptr   Configuration backup setup structure. See
 *                    @ref DL_QSPI_backupConfig.
 *
 *  @retval     FALSE if a configuration does not exist in ptr (will not be
 *              loaded). TRUE if a configuration successfully loaded
 *
 *  @sa         DL_QSPI_saveConfiguration
 */
bool DL_QSPI_restoreConfiguration(QSPI_Regs *qspi, DL_QSPI_backupConfig *ptr);


/**
 *  @brief      Selects which port is used for QSPI memory access
 *
 *  This API selects which port is used for QSPI memory access.
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 *  @param[in]  port           Port selection
 */
__STATIC_INLINE void DL_QSPI_setPort(QSPI_Regs *qspi, DL_QSPI_PORT port)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL0, port, QSPI_QSPICTL0_QSPIMODE_MASK);
}


/**
 *  @brief      Enables the performance mode byte insertion
 *
 *  This API enables the performance mode byte insertion
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 */
__STATIC_INLINE void DL_QSPI_enablePerformanceMode(QSPI_Regs *qspi)
{
    qspi->IPSPECIFIC_QSPI.QSPICTL0 |= QSPI_QSPICTL0_QSPIPERFMODE_ENABLE;
}


/**
 *  @brief      Disables the performance mode byte insertion
 *
 *  This API disables the performance mode byte insertion.
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 */
__STATIC_INLINE void DL_QSPI_disablePerformanceMode(QSPI_Regs *qspi)
{
    /* Clear the QSPI performance mode bit */
    qspi->IPSPECIFIC_QSPI.QSPICTL0 &= ~QSPI_QSPICTL0_QSPIPERFMODE_ENABLE;
}

/**
 *  @brief      Sets the number of address byte
 *
 *   Sets the number of address byte to be used for communication with QSPI flash during a data read
 *
 *  @param[in]  qspi           Pointer to the register overlay for the
 *                            peripheral
 *  @param[in]  addressMode    Address mode. One of @ref DL_QSPI_ADDRESS_MODE
 */
__STATIC_INLINE void DL_QSPI_setAddressMode(QSPI_Regs *qspi, DL_QSPI_ADDRESS_MODE addressMode)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL0, addressMode, QSPI_QSPICTL0_QSPIADDRMODE_MASK);
}



/**
 * @brief Set the bus format
 *
 * This function sets the bus format used for QSPI communications.
 *
 * @param qspi    Pointer to the register overlay for the peripheral
 * @param busFormat  Bus format. One of @ref DL_QSPI_BUS_FORMAT
 *
 */
__STATIC_INLINE void DL_QSPI_setBusFormat(QSPI_Regs *qspi, DL_QSPI_BUS_FORMAT busFormat)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL0, busFormat, QSPI_QSPICTL0_QSPIFORMAT_MASK);
}


/**
 * @brief Provide the performance byte to be used during a flash read
 *
 * This function sets the performance byte to be used for communication with QSPI flash during a data read
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param performanceByte    Performance byte. The value of this byte is used to configure the number of dummy clock cycles.
 *
 * @note The default value of the QSPIPERFBYTE field is 0.
 */
__STATIC_INLINE void DL_QSPI_setPerformanceByte(QSPI_Regs *qspi, uint32_t performanceByte)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL0, performanceByte << QSPI_QSPICTL0_QSPIPERFBYTE_OFS
    , QSPI_QSPICTL0_QSPIPERFBYTE_MASK);
}


/**
 * @brief Provides the instruction opcode to be used during a flash read
 *
 * This function provides the instruction opcode to be used during a flash read
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param commandByte   Command byte. The value of this byte is used to configure the QSPI module to send the
 *                       specified command during data transfer.
 *
 * @note The default value of the QSPICMDBYTE field is 0.
 */
__STATIC_INLINE void DL_QSPI_setCommandByte(QSPI_Regs *qspi, uint32_t commandByte)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL0, commandByte << QSPI_QSPICTL0_QSPICMDBYTE_OFS
    , QSPI_QSPICTL0_QSPICMDBYTE_MASK);
}

/**
 * @brief Set the word size
 *
 * Set the size of the word used for communication with QSPI flash during a data read
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param size          Word size. The value of this parameter is used to configure the number of bits used
 *                       in a transfer.
 *
 * @note The default value of the QSPIDSIZE field is QSPI_QSPICTL0_QSPIDSIZE__8BITDSIZE.
 */
__STATIC_INLINE void DL_QSPI_setWordSize(QSPI_Regs *qspi, DL_QSPI_WORD_SIZE size)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL0, size, QSPI_QSPICTL0_QSPIDSIZE_MASK);
}



/**
 * @brief Set the number of dummy clock cycles
 *
 * Set the number of dummy clock cycles to be used during a flash read.
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param numDummyClock Number of dummy clock cycles. 
 *
 * @note The default value of the QSPIDUMMYCLK field is 0.
 */
__STATIC_INLINE void DL_QSPI_setDummyClock(QSPI_Regs *qspi, uint32_t numDummyClock)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL0, numDummyClock << QSPI_QSPICTL0_QSPIDUMMYCLK_OFS
    , QSPI_QSPICTL0_QSPIDUMMYCLK_MASK);
}


/**
 * @brief Enable QSPI prefetch
 *
 * Enable the prefetch feature of the QSPI.
 *
 * @param qspi  Pointer to the register overlay for the peripheral
 *
 */
__STATIC_INLINE void DL_QSPI_enablePreFetch(QSPI_Regs *qspi)
{
    qspi->IPSPECIFIC_QSPI.QSPICTL1 |= QSPI_QSPICTL1_QSPIPREFETCH_ENABLE;
}

/**
 * @brief Disable QSPI prefetch
 *
 * Disable the prefetch feature of the QSPI.
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 *
 */
__STATIC_INLINE void DL_QSPI_disablePreFetch(QSPI_Regs *qspi)
{
    /* Disable prefetch feature of the QSPI */
    qspi->IPSPECIFIC_QSPI.QSPICTL1 &= ~QSPI_QSPICTL1_QSPIPREFETCH_ENABLE;
}


/**
 * @brief Contains the number of receive bytes to write to the TXFIFO when QSPIPREFETCH is enabled
 *
 * Contains the number of receive bytes to write to the TXFIFO when QSPIPREFETCH is enabled
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param RXByteCount   Number of bytes to receive
 *
 * @note The default value of the RXCOUNT field is 0.
 */
__STATIC_INLINE void DL_QSPI_setRXCount(QSPI_Regs *qspi, uint32_t RXByteCount)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL1, RXByteCount << QSPI_QSPICTL1_RXCOUNT_OFS,
        QSPI_QSPICTL1_RXCOUNT_MASK);
}


/**
 * @brief Controls the number of transmit bytes to send during a configuration or status frame.
 *
 * Controls the number of transmit bytes to send during a configuration or status frame
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param TXByteCount   Number of bytes to transmit
 *
 * @note The default value of the TXCOUNT field is 0.
 */
__STATIC_INLINE void DL_QSPI_setTXCount(QSPI_Regs *qspi, uint32_t TXByteCount)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPICTL1, TXByteCount << QSPI_QSPICTL1_TXCOUNT_OFS,
        QSPI_QSPICTL1_TXCOUNT_MASK);
}


/**
 * @brief Set the number of bus clock cycles to hold the chip select high
 *
 * When in performance mode, the parameter provides the number of bus clocks for which the CS shall be 
 * high when an address change is detected
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param numBusClock   Number of bus clock cycles to hold the chip select high
 *
 * @note The default value of the CSHIGHTIME field is 0.
 */
__STATIC_INLINE void DL_QSPI_setChipSelectHighTime(QSPI_Regs *qspi, uint32_t numBusClock)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPITIMING, numBusClock << QSPI_QSPITIMING_CSHIGHTIME_OFS,
        QSPI_QSPITIMING_CSHIGHTIME_MASK);
}


/**
 * @brief Controls the delay in internal SCLK to sample data.
 *
 * Controls the delay in internal SCLK to sample data.
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param delay         Delay in SCLK cycles
 *
 * @note The default value of the DATASAMPLDLY field is DL_QSPI_DATA_SAMPLE_DELAY_0.
 */
__STATIC_INLINE void DL_QSPI_setDataSampleDelay(QSPI_Regs *qspi, DL_QSPI_DATA_SAMPLE_DELAY delay)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPITIMING, delay,
        QSPI_QSPITIMING_DATASAMPLDLY_MASK);
}



/**
 * @brief Controls the number of serial clock edge after chip select is asserted to start the SCLK
 *
 * controls the number of serial clock edge after chip select is asserted to start the SCLK
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param delay         Delay in SCLK cycles
 *
 * @note The default value of the CSASSERTDLY field is DL_QSPI_CHIP_SELECT_ASSERT_DELAY_0P5.
 */
__STATIC_INLINE void DL_QSPI_setChipSelectAssertDelay(QSPI_Regs *qspi, DL_QSPI_CHIP_SELECT_ASSERT_DELAY delay)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPITIMING, delay,
        QSPI_QSPITIMING_CSASSERTDLY_MASK);
}


/**
 * @brief Controls the number of serial clock edge after last SCLK to deassert the chip select
 *
 * controls the number of serial clock edge after last SCLK to deassert the chip select
 *
 * @param qspi          Pointer to the register overlay for the peripheral
 * @param delay         Delay in SCLK cycles
 *
 * @note The default value of the CSDEASSERTDLY field is DL_QSPI_CHIP_SELECT_DEASSERT_DELAY_0P5.
 */
__STATIC_INLINE void DL_QSPI_setChipSelectDeAssertDelay(QSPI_Regs *qspi, DL_QSPI_CHIP_SELECT_DEASSERT_DELAY delay)
{
    DL_Common_updateReg(&qspi->IPSPECIFIC_QSPI.QSPITIMING, delay,
        QSPI_QSPITIMING_CSDEASSERTDLY_MASK);
}


/**
 * @brief Flushes the TX FIFO
 *
 * Flushes the TX FIFO
 *
 * @param qspi    Pointer to the QSPI peripheral register overlay
 */
__STATIC_INLINE void DL_QSPI_flushTXFIFO(QSPI_Regs *qspi)
{
    qspi->IPSPECIFIC_QSPI.QSPIFIFOCTL |= QSPI_QSPIFIFOCTL_TXFIFOFLUSH_CLEAR;
}


/**
 * @brief Flushes the RX FIFO
 *
 * Flushes the RX FIFO
 *
 * @param qspi    Pointer to the QSPI peripheral register overlay
 */
__STATIC_INLINE void DL_QSPI_flushRXFIFO(QSPI_Regs *qspi)
{
    qspi->IPSPECIFIC_QSPI.QSPIFIFOCTL |= QSPI_QSPIFIFOCTL_RXFIFOFLUSH_CLEAR;
}

/**
 * @brief Deasserts the chip select
 *
 * Controls the chip select hold feature during an active transaction when QSPIFORMAT is not 0x0
 *
 * @param qspi    Pointer to the QSPI peripheral register overlay
 *
 */
__STATIC_INLINE void DL_QSPI_deAssertChipSelect(QSPI_Regs *qspi)
{
    qspi->IPSPECIFIC_QSPI.QSPICSCTL |= (QSPI_QSPICSCTL_CSHOLDCTL_DEASSERT);
}

/**
 * @brief Checks if the chip select is de-asserted
 *
 * Checks if the chip select is de-asserted.
 *
 * @param qspi pointer to the register overlay for the peripheral
 *
 * @return Returns true if the chip select is de-asserted, false otherwise
 */
__STATIC_INLINE bool DL_QSPI_isChipSelectDeAsserted(QSPI_Regs *qspi)
{
    return ((qspi->IPSPECIFIC_QSPI.QSPICSCTL & QSPI_QSPICSCTL_CSHOLDCTL_MASK) == QSPI_QSPICSCTL_CSHOLDCTL_DEASSERT);
}



/**
 * @brief Checks if the QSPI is in idle state
 *
 * Provides the state of the QSPI controller
 *
 * @param qspi pointer to the register overlay for the peripheral
 *
 * @return Returns the idle status of the QSPI
 *
 * @retval true The QSPI is in idle state
 * @retval false The QSPI is not in idle state
 */
__STATIC_INLINE bool DL_QSPI_isQSPIIdle(QSPI_Regs *qspi)
{
    return ((qspi->IPSPECIFIC_QSPI.QSPISTATUS & QSPI_QSPISTATUS_QSPIIDLE_MASK) == QSPI_QSPISTATUS_QSPIIDLE_QSPIIDLE_IDLE);
}



/**
 * @brief Checks if the QSPI is in stall state
 *
 * The bit indicates the state of the QSPI controller during an active transaction
 *
 * @param qspi pointer to the register overlay for the peripheral
 *
 * @return Returns the stall status of the QSPI
 *
 * @retval true The QSPI is in stall state
 * @retval false The QSPI is not in stall state
 */

__STATIC_INLINE bool DL_QSPI_isQSPIStalled(QSPI_Regs *qspi)
{
    return ((qspi->IPSPECIFIC_QSPI.QSPISTATUS & QSPI_QSPISTATUS_QSPISTALL_MASK) == QSPI_QSPISTATUS_QSPISTALL_QSPISTALL_STALL);
}

/**
 * @brief Checks if the QSPI TX FIFO is full
 *
 * The bit indicates if the QSPI TX FIFO is full
 *
 * @param qspi pointer to the register overlay for the peripheral
 *
 * @return Returns the full status of the QSPI TX FIFO
 *
 * @retval true The QSPI TX FIFO is full
 * @retval false The QSPI TX FIFO is not full
 */
__STATIC_INLINE bool DL_QSPI_isQSPITXFIFOFull(QSPI_Regs *qspi)
{
    return ((qspi->IPSPECIFIC_QSPI.QSPISTATUS & QSPI_QSPISTATUS_TXFIFONF_MASK) == QSPI_QSPISTATUS_TXFIFONF_TXFIFONF_F);
}


/**
 * @brief Checks if the QSPI TX FIFO is empty
 *
 * The bit indicates if the QSPI TX FIFO is empty
 *
 * @param qspi pointer to the register overlay for the peripheral
 *
 * @return Returns the empty status of the QSPI TX FIFO
 *
 * @retval true The QSPI TX FIFO is empty
 * @retval false The QSPI TX FIFO is not empty
 */
__STATIC_INLINE bool DL_QSPI_isQSPITXFIFOEmpty(QSPI_Regs *qspi)
{
    return ((qspi->IPSPECIFIC_QSPI.QSPISTATUS & QSPI_QSPISTATUS_TXFIFOE_MASK) == QSPI_QSPISTATUS_TXFIFOE_TXFIFOE_E);
}



/**
 * @brief Checks if the QSPI RX FIFO is full
 *
 * The bit indicates if the QSPI RX FIFO is full
 *
 * @param qspi pointer to the register overlay for the peripheral
 *
 * @return Returns the full status of the QSPI RX FIFO
 *
 * @retval true The QSPI RX FIFO is full
 * @retval false The QSPI RX FIFO is not full
 */
__STATIC_INLINE bool DL_QSPI_isQSPIRXFIFOFull(QSPI_Regs *qspi)
{
    return ((qspi->IPSPECIFIC_QSPI.QSPISTATUS & QSPI_QSPISTATUS_RXFIFONF_MASK) == QSPI_QSPISTATUS_RXFIFONF_RXFIFONF_F);
}


/**
 * @brief Checks if the QSPI RX FIFO is empty
 *
 * The bit indicates if the QSPI RX FIFO is empty
 *
 * @param qspi pointer to the register overlay for the peripheral
 *
 * @return Returns the empty status of the QSPI RX FIFO
 *
 * @retval true The QSPI RX FIFO is empty
 * @retval false The QSPI RX FIFO is not empty
 *
 */
__STATIC_INLINE bool DL_QSPI_isQSPIRXFIFOEmpty(QSPI_Regs *qspi)
{
    return ((qspi->IPSPECIFIC_QSPI.QSPISTATUS & QSPI_QSPISTATUS_RXFIFOE_MASK) == QSPI_QSPISTATUS_RXFIFOE_RXFIFOE_E);
}



#ifdef __cplusplus
}
#endif

#endif /* __MSP_HAS_QSPI__ */

#endif /* ti_dl_dl_qspi__include */
/** @}*/
