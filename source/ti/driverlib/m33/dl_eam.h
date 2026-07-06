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
 * PURPOSE ARE DISCLAIMED. IN NO SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       dl_eam.h
 *  @brief      EAM(Error Aggregator Module) Peripheral Interface
 *  @defgroup   Error Aggregator Module (EAM)
 *  @anchor ti_devices_msp_dl_eam_Overview
 *  # Overview
 *
 *
 *  <hr>
 ******************************************************************************
 */
/** @addtogroup EAM
 * @{
 */
#ifndef ti_dl_dl_eam__include
#define ti_dl_dl_eam__include

#include <stdbool.h>
#include <stdint.h>

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

#ifdef __MSP_HAS_EAM__

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */

 /**
  * @brief EAM SECURITY ERROR CLEAR MASK
  */
 #define DL_EAM_SECURITY_ERR_MASK                                (0x00000FFFU)
 /**
  * @brief EAM FRI SEC CLEAR MASK
  */
 #define DL_EAM_FRI_SEC_MASK                                     (0x00000001U)
 /**
  * @brief EAM FRI DED CLEAR MASK
  */
 #define DL_EAM_FRI_DED_MASK                                     (0x00000003U)
 /**
  * @brief EAM SYSMEM SEC CLEAR MASK
  */
 #define DL_EAM_SYSMEM_SEC_MASK                                  (0x00000003U)
 /**
  * @brief EAM SYSMEM DED CLEAR MASK
  */
 #define DL_EAM_SYSMEM_DED_MASK                                  (0x00000003U)

 /**
  * @brief EAM SECURITY ERROR ADDRESS RESET VALUE
  */
 #define DL_EAM_SECURITY_ERR_FLAG_NO_ERROR                       (0x00000000U)

 /**
  * @brief EAM FRI SEC ADDRESS RESET VALUE
  */
 #define DL_EAM_FRI_SEC_FLAG_NO_ERROR                            (0x00000000U)
 /**
  * @brief EAM FRI DED ADDRESS RESET VALUE
  */
 #define DL_EAM_FRI_DED_FLAG_NO_ERROR                           (0x00000000U)
 /**
  * @brief EAM SYSMEM SEC ADDRESS RESET VALUE
  */
 #define DL_EAM_SYSMEM_SEC_FLAG_NO_ERROR                         (0x00000000U)
 /**
  * @brief EAM SYSMEM DED ADDRESS RESET VALUE
  */
 #define DL_EAM_SYSMEM_DED_FLAG_NO_ERROR                         (0x00000000U)

/** @enum DL_EAM_SECURITY_ERROR_FLAG */
typedef enum {
    /* !< Secure error flag */
    DL_EAM_SECURITY_ERROR_SECURE = EAM_SECURITY_ERR_FLAG_SECURE_MASK,
     /* !< Privilege error flag */
    DL_EAM_SECURITY_ERROR_PRIV = EAM_SECURITY_ERR_FLAG_PRIV_MASK,
    /* !< Hide protection error flag */
    DL_EAM_SECURITY_ERROR_HDP = EAM_SECURITY_ERR_FLAG_HDP_MASK,
    /* !< FPI ILLADDR flag */
    DL_EAM_SECURITY_ERROR_FPI_ILLADDR = EAM_SECURITY_ERR_FLAG_FPI_ILLADDR_MASK,
     /* !< FPI ILLPROG flag */
    DL_EAM_SECURITY_ERROR_FPI_ILLPROG = EAM_SECURITY_ERR_FLAG_FPI_ILLPROG_MASK,
     /* !< FPI ILLERASE flag */
    DL_EAM_SECURITY_ERROR_FPI_ILLERASE = EAM_SECURITY_ERR_FLAG_FPI_ILLERASE_MASK,
    /* !< FPI ILLRDVER flag */
    DL_EAM_SECURITY_ERROR_FPI_ILLRDVER = EAM_SECURITY_ERR_FLAG_FPI_ILLRDVER_MASK,
    /* !< FPI ILLMODECH flag */
    DL_EAM_SECURITY_ERROR_FPI_ILLMODECH = EAM_SECURITY_ERR_FLAG_FPI_ILLMODECH_MASK,
    /* !< FPI ILLCMD flag */
    DL_EAM_SECURITY_ERROR_FPI_ILLCMD = EAM_SECURITY_ERR_FLAG_FPI_ILLCMD_MASK,
    /* !< FPI ILLSIZE flag */
    DL_EAM_SECURITY_ERROR_FPI_ILLSIZE = EAM_SECURITY_ERR_FLAG_FPI_ILLSIZE_MASK,
    /* !< FLC MMR ACCESS ERROR flag */
    DL_EAM_SECURITY_ERROR_FLC_MMR_ACCESS_ERROR  =  EAM_SECURITY_ERR_FLAG_FLC_MMR_ACCESS_ERROR_MASK,
    /* !< FLSEM ACCESS ERROR flag */
    DL_EAM_SECURITY_ERROR_FLSEM_ACCESS_ERROR = EAM_SECURITY_ERR_FLAG_FLSEM_ACCESS_ERROR_MASK,
} DL_EAM_SECURITY_ERROR_FLAG;

/** @enum DL_EAM_FRI_SEC_FLAG */
typedef enum {
     /* !< Single error correction error */
    DL_EAM_FRI_SEC = EAM_FRI_SEC_FLAG_SEC_MASK,
} DL_EAM_FRI_SEC_FLAG;

/** @enum DL_EAM_FRI_DED_FLAG */
typedef enum {
    /* !< Double error detect error */
    DL_EAM_FRI_DED = EAM_FRI_DED_FLAG_DED_MASK,
    /* !< diagnostic Double error detect error */
    DL_EAM_FRI_DIAG_DED = EAM_FRI_DED_FLAG_DIAG_DED_MASK,
} DL_EAM_FRI_DED_FLAG;

/** @enum DL_EAM_SYSMEM_SEC_FLAG */
typedef enum {
    /* !< Single error correction during read */
    DL_EAM_SYSMEM_RD_SEC = EAM_SYSMEM_SEC_FLAG_RD_SEC_MASK,
    /* !< Single error correction during write */
    DL_EAM_SYSMEM_WR_SEC = EAM_SYSMEM_SEC_FLAG_WR_SEC_MASK,
} DL_EAM_SYSMEM_SEC_FLAG;

/** @enum DL_EAM_SYSMEM_DED_FLAG */
typedef enum {
    /* !< Double error detect error during read */
    DL_EAM_SYSMEM_RD_DED = EAM_SYSMEM_DED_FLAG_RD_DED_MASK,
    /* !< Double error detect error during write */
    DL_EAM_SYSMEM_WR_DED = EAM_SYSMEM_DED_FLAG_WR_DED_MASK,
} DL_EAM_SYSMEM_DED_FLAG;

/**
 *  @brief  Return value that is stored in SECURITY ERROR FLAG.
 *
 *  @return The cause of security error. One of @ref DL_EAM_SECURITY_ERROR_FLAG
 */
__STATIC_INLINE DL_EAM_SECURITY_ERROR_FLAG DL_EAM_getSecurityErrorFlag(EAM_Regs *eam)
{
    return (DL_EAM_SECURITY_ERROR_FLAG) (eam->EAM_REGS.SECURITY_ERR_FLAG  & DL_EAM_SECURITY_ERR_MASK);
}

 /**
  *  @brief      Clear Security Error Flag
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  */
 __STATIC_INLINE void DL_EAM_clearSecurityErrorFlag(EAM_Regs *eam)
 {
    eam->EAM_REGS.SECURITY_ERR_CLR = (eam->EAM_REGS.SECURITY_ERR_FLAG & DL_EAM_SECURITY_ERR_MASK);
 }


 /**
  *  @brief      Get Security Error Address
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return Security Error Address Value
  *
  */
 __STATIC_INLINE uint32_t DL_EAM_getSecurityErrorAddress(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.SECURITY_ERR_ADDR);
 }

 /**
  *  @brief      Get Security Error MSTID
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return Security Error Master ID Value
    */
 __STATIC_INLINE uint8_t DL_EAM_getSecurityErrorMSTID(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.SECURITY_ERR_MSTID);
 }


 /**
  *  @brief      Clear FRI SEC Flag
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  */
__STATIC_INLINE void DL_EAM_clearFRISECFlag(EAM_Regs *eam)
 {
    eam->EAM_REGS.FRI_SEC_CLR = (eam->EAM_REGS.FRI_SEC_FLAG & DL_EAM_FRI_SEC_MASK);
 }

/**
 *  @brief  Return value that is stored in FRI SEC FLAG.
 *
 *  @return The cause of FRI SEC . One of @ref DL_EAM_FRI_SEC_FLAG
 */
__STATIC_INLINE DL_EAM_FRI_SEC_FLAG DL_EAM_getFRISECFlag(EAM_Regs *eam)
{
    return (DL_EAM_FRI_SEC_FLAG) (eam->EAM_REGS.FRI_SEC_FLAG & DL_EAM_FRI_SEC_MASK);
}

 /**
  *  @brief      Get FRI SEC Address
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return FRI SEC Address Value
  *
  */
 __STATIC_INLINE uint32_t DL_EAM_getFRISECAddress(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.FRI_SEC_ADDR);
 }

 /**
  *  @brief      Get FRI SEC MSTID
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return FRI SEC Master ID Value
    */
 __STATIC_INLINE uint8_t DL_EAM_getFriSECMSTID(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.FRI_SEC_MSTID);
 }

  /**
  *  @brief      Clear FRI DED Flag
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  */
 __STATIC_INLINE void DL_EAM_clearFRIDEDFlag(EAM_Regs *eam)
 {
    eam->EAM_REGS.FRI_DED_CLR = (eam->EAM_REGS.FRI_DED_FLAG & DL_EAM_FRI_DED_MASK);
 }

/**
 *  @brief  Return value that is stored in FRI DED FLAG.
 *
 *  @return The cause of FRI DED . One of @ref DL_EAM_FRI_DED_FLAG
 */
__STATIC_INLINE DL_EAM_FRI_DED_FLAG DL_EAM_getFRIDEDFlag(EAM_Regs *eam)
{
    return (DL_EAM_FRI_DED_FLAG) (eam->EAM_REGS.FRI_DED_FLAG & DL_EAM_FRI_DED_MASK);
}

 /**
  *  @brief      Get FRI DED Address
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return FRI DED Address Value
  *
  */
 __STATIC_INLINE uint32_t DL_EAM_getFRIDEDAddress(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.FRI_DED_ADDR);
 }

 /**
  *  @brief      Get FRI DED MSTID
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return FRI DED Master ID Value
    */
 __STATIC_INLINE uint8_t DL_EAM_getFRIDEDMSTID(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.FRI_DED_MSTID);
 }

 /**
  *  @brief      Clear SYSMEM SEC Flag
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  */
 __STATIC_INLINE void DL_EAM_clearSYSMEMSECFlag(EAM_Regs *eam)
 {
    eam->EAM_REGS.SYSMEM_SEC_CLR = (eam->EAM_REGS.SYSMEM_SEC_FLAG & DL_EAM_SYSMEM_SEC_MASK);
 }

/**
 *  @brief  Return value that is stored in SYSMEM SEC FLAG.
 *
 *  @return The cause of SYSMEM SEC . One of @ref DL_EAM_SYSMEM_SEC_FLAG
 */
__STATIC_INLINE DL_EAM_SYSMEM_SEC_FLAG DL_EAM_getSYSMEMSECFlag(EAM_Regs *eam)
{
    return (DL_EAM_SYSMEM_SEC_FLAG) (eam->EAM_REGS.SYSMEM_SEC_FLAG & DL_EAM_SYSMEM_SEC_MASK);
}

 /**
  *  @brief      Get SYSMEM SEC Address
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return SYSMEM SEC Address Value
  *
  */
 __STATIC_INLINE uint32_t DL_EAM_getSYSMEMSECAddress(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.SYSMEM_SEC_ADDR);
 }

 /**
  *  @brief      Get SYSMEM SEC MSTID
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return SYSMEM SEC Master ID Value
    */
 __STATIC_INLINE uint8_t DL_EAM_getSYSMEMSECMSTID(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.SYSMEM_SEC_MSTID);
 }

 /**
  *  @brief      Clear SYSMEM DED Flag
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  */
 __STATIC_INLINE void DL_EAM_clearSYSMEMDEDFlag(EAM_Regs *eam)
 {
    eam->EAM_REGS.SYSMEM_DED_CLR = (eam->EAM_REGS.SYSMEM_DED_FLAG & DL_EAM_SYSMEM_DED_MASK);
 }

/**
 *  @brief  Return value that is stored in SYSMEM DED FLAG.
 *
 *  @return The cause of SYSMEM DED . One of @ref DL_EAM_SYSMEM_DED_FLAG
 */
__STATIC_INLINE DL_EAM_SYSMEM_DED_FLAG DL_EAM_getSYSMEMDEDFlag(EAM_Regs *eam)
{
    return (DL_EAM_SYSMEM_DED_FLAG) (eam->EAM_REGS.SYSMEM_DED_FLAG & DL_EAM_SYSMEM_DED_MASK);
}

 /**
  *  @brief      Get SYSMEM DED Address
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return SYSMEM DED Address Value
  *
  */
 __STATIC_INLINE uint32_t DL_EAM_getSYSMEMDEDAddress(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.SYSMEM_DED_ADDR);
 }

 /**
  *  @brief      Get SYSMEM DED MSTID
  *
  *  @param[in]  eam  Pointer to the register overlay for the peripheral
  *
  *  @return     return SYSMEM DED Master ID Value
    */
 __STATIC_INLINE uint8_t DL_EAM_getSYSMEMDEDMSTID(EAM_Regs *eam)
 {
    return (eam->EAM_REGS.SYSMEM_DED_MSTID);
 }
#ifdef __cplusplus

 }
#endif

#endif /* __MSP_HAS_EAM__ */

#endif /* ti_dl_dl_eam__include */
/** @}*/
