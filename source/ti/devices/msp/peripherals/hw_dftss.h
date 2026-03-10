/*****************************************************************************

  Copyright (C) 2025 Texas Instruments Incorporated - http://www.ti.com/ 

  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions 
  are met:

   Redistributions of source code must retain the above copyright 
   notice, this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the 
   documentation and/or other materials provided with the   
   distribution.

   Neither the name of Texas Instruments Incorporated nor the names of
   its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/

#ifndef ti_devices_msp_peripherals_hw_dftss__include
#define ti_devices_msp_peripherals_hw_dftss__include

/* This preliminary header file does not have a version number */
/* Filename: hw_dftss.h */
/* MMR revised: 2025-04-09 14:12:34 */
/* MMR repo: https://bitbucket.itg.ti.com/projects/cmcu_msp65soc/repos/m33c1 */
/* MMR revision: f4602b816b1643a266cc7aae6be755d35f394359 */
/* Generator revision: ee5e56ed8e50e22306abbffa1ce64d23abbf32b9
   (MInT: 1818019a64e181dbbfd48fdc69fea15367af4c13) */

#ifndef __CORTEX_M
  #ifdef __cplusplus
    #define __I  volatile        /*!< Defines 'read only' permissions */
  #else
    #define __I  volatile const  /*!< Defines 'read only' permissions */
  #endif
  #define __O  volatile          /*!< Defines 'write only' permissions */
  #define __IO  volatile         /*!< Defines 'read / write' permissions */

  /* following defines should be used for structure members */
  #define __IM  volatile const   /*! Defines 'read only' structure member permissions */
  #define __OM  volatile         /*! Defines 'write only' structure member permissions */
  #define __IOM  volatile        /*! Defines 'read / write' structure member permissions */
#endif

/* Use standard integer types with explicit width */
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#if defined ( __CC_ARM )
#pragma anon_unions
#endif

/******************************************************************************
* DFTSS Registers
******************************************************************************/
#define DFTSS_PINCM_OFS                          ((uint32_t)0x00000004U)


/** @addtogroup DFTSS_PINCM
  @{
*/

typedef struct {
  __IO uint32_t TDI;                               /* !< (@ 0x00000004) TDI */
  __IO uint32_t TDO;                               /* !< (@ 0x00000008) TDO */
} DFTSS_PINCM_Regs;

/*@}*/ /* end of group DFTSS_PINCM */

/** @addtogroup DFTSS
  @{
*/

typedef struct {
       uint32_t RESERVED0;
  DFTSS_PINCM_Regs  PINCM;                             /* !< (@ 0x00000004) */
} DFTSS_Regs;

/*@}*/ /* end of group DFTSS */



#if defined ( __CC_ARM )
#pragma no_anon_unions
#endif

/******************************************************************************
* DFTSS Register Control Bits
******************************************************************************/

/* DFTSS_TDI Bits */
/* DFTSS_TDI[PSTATE] Bits */
#define DFTSS_TDI_PSTATE_OFS                     (6)                             /* !< PSTATE Offset */
#define DFTSS_TDI_PSTATE_MASK                    ((uint32_t)0x000000C0U)         /* !< Peripheral-Analog Channel State */
#define DFTSS_TDI_PSTATE_UNASSIGNED              ((uint32_t)0x00000000U)         /* !< P-Channel is in Unassigned State */
#define DFTSS_TDI_PSTATE_HANDOVER                ((uint32_t)0x00000040U)         /* !< P-Channel is in Handover State */
#define DFTSS_TDI_PSTATE_CONC_NOT_LOCKED         ((uint32_t)0x00000080U)         /* !< P-Channel is in Connected State and
                                                                                    not Locked (That is F field is
                                                                                    allowed to change without going back
                                                                                    through Unassigned state) */
#define DFTSS_TDI_PSTATE_CONC_LOCKED             ((uint32_t)0x000000C0U)         /* !< P-Channel is in Connected State and
                                                                                    Locked (That is F field is not
                                                                                    allowed to change to a different
                                                                                    non-Zero value until both G and P
                                                                                    channels go to Unassigned) */
/* DFTSS_TDI[GSTATE] Bits */
#define DFTSS_TDI_GSTATE_OFS                     (14)                            /* !< GSTATE Offset */
#define DFTSS_TDI_GSTATE_MASK                    ((uint32_t)0x0000C000U)         /* !< GPIO Channel State */
#define DFTSS_TDI_GSTATE_UNASSIGNED              ((uint32_t)0x00000000U)         /* !< G-Channel is in Unassigned State */
#define DFTSS_TDI_GSTATE_HANDOVER                ((uint32_t)0x00004000U)         /* !< G-Channel is in Handover State */
#define DFTSS_TDI_GSTATE_CONC_NOT_LOCKED         ((uint32_t)0x00008000U)         /* !< G-Channel is in Connected State and
                                                                                    not Locked (That is F field is
                                                                                    allowed to change without going back
                                                                                    through Unassigned state) */
#define DFTSS_TDI_GSTATE_CONC_LOCKED             ((uint32_t)0x0000C000U)         /* !< G-Channel is in Connected State and
                                                                                    Locked (That is F field is not
                                                                                    allowed to change to a different
                                                                                    non-Zero value until both G and P
                                                                                    channels go to Unassigned) */
/* DFTSS_TDI[PIPD] Bits */
#define DFTSS_TDI_PIPD_OFS                       (16)                            /* !< PIPD Offset */
#define DFTSS_TDI_PIPD_MASK                      ((uint32_t)0x00010000U)         /* !< Pull down enable */
#define DFTSS_TDI_PIPD_NO_PULL_DOWN              ((uint32_t)0x00000000U)         /* !< No pull down */
#define DFTSS_TDI_PIPD_PULL_DOWN                 ((uint32_t)0x00010000U)         /* !< Pull down */
/* DFTSS_TDI[PIPU] Bits */
#define DFTSS_TDI_PIPU_OFS                       (17)                            /* !< PIPU Offset */
#define DFTSS_TDI_PIPU_MASK                      ((uint32_t)0x00020000U)         /* !< Pull up enable */
#define DFTSS_TDI_PIPU_NO_PULL_UP                ((uint32_t)0x00000000U)         /* !< No pull up */
#define DFTSS_TDI_PIPU_PULL_UP                   ((uint32_t)0x00020000U)         /* !< Pull up */
/* DFTSS_TDI[INENA] Bits */
#define DFTSS_TDI_INENA_OFS                      (18)                            /* !< INENA Offset */
#define DFTSS_TDI_INENA_MASK                     ((uint32_t)0x00040000U)         /* !< Input enable */
#define DFTSS_TDI_INENA_INP_DISABLE              ((uint32_t)0x00000000U)         /* !< Inputs 0 to connected core */
#define DFTSS_TDI_INENA_INP_ENABLE               ((uint32_t)0x00040000U)         /* !< Inputs IO pad value to connected
                                                                                    core */
/* DFTSS_TDI[HYSTEN] Bits */
#define DFTSS_TDI_HYSTEN_OFS                     (19)                            /* !< HYSTEN Offset */
#define DFTSS_TDI_HYSTEN_MASK                    ((uint32_t)0x00080000U)         /* !< Hysteresis enable */
#define DFTSS_TDI_HYSTEN_DIS_HYST                ((uint32_t)0x00000000U)         /* !< No hysteresis */
#define DFTSS_TDI_HYSTEN_EN_HYST                 ((uint32_t)0x00080000U)         /* !< Hysteresis on */
/* DFTSS_TDI[DRV] Bits */
#define DFTSS_TDI_DRV_OFS                        (20)                            /* !< DRV Offset */
#define DFTSS_TDI_DRV_MASK                       ((uint32_t)0x00700000U)         /* !< Drive strength options */
#define DFTSS_TDI_DRV_STRENGTH_0                 ((uint32_t)0x00000000U)         /* !< Lowest drive strength */
#define DFTSS_TDI_DRV_STRENGTH_1                 ((uint32_t)0x00100000U)         /* !< Drive strength 2/8 */
#define DFTSS_TDI_DRV_STRENGTH_2                 ((uint32_t)0x00200000U)         /* !< Drive strength 3/8 */
#define DFTSS_TDI_DRV_STRENGTH_3                 ((uint32_t)0x00300000U)         /* !< Drive strength 4/8 */
#define DFTSS_TDI_DRV_STRENGTH_4                 ((uint32_t)0x00400000U)         /* !< Drive strength 5/8 */
#define DFTSS_TDI_DRV_STRENGTH_5                 ((uint32_t)0x00500000U)         /* !< Drive strength 6/8 */
#define DFTSS_TDI_DRV_STRENGTH_6                 ((uint32_t)0x00600000U)         /* !< Drive strength 7/8 */
#define DFTSS_TDI_DRV_STRENGTH_7                 ((uint32_t)0x00700000U)         /* !< Highest drive strength */
/* DFTSS_TDI[HIGHZ0] Bits */
#define DFTSS_TDI_HIGHZ0_OFS                     (24)                            /* !< HIGHZ0 Offset */
#define DFTSS_TDI_HIGHZ0_MASK                    ((uint32_t)0x01000000U)         /* !< High-Z instead of low output */
#define DFTSS_TDI_HIGHZ0_DRV_LOW                 ((uint32_t)0x00000000U)         /* !< Pin can be driven low */
#define DFTSS_TDI_HIGHZ0_DRV_HIZ                 ((uint32_t)0x01000000U)         /* !< Pin is tri-stated instead of driven
                                                                                    low */
/* DFTSS_TDI[HIGHZ1] Bits */
#define DFTSS_TDI_HIGHZ1_OFS                     (25)                            /* !< HIGHZ1 Offset */
#define DFTSS_TDI_HIGHZ1_MASK                    ((uint32_t)0x02000000U)         /* !< High-Z instead of high output */
#define DFTSS_TDI_HIGHZ1_DRV_HIGH                ((uint32_t)0x00000000U)         /* !< Pin can be driven high */
#define DFTSS_TDI_HIGHZ1_DRV_HIZ                 ((uint32_t)0x02000000U)         /* !< Pin is tri-stated instead of driven
                                                                                    high */
/* DFTSS_TDI[INV] Bits */
#define DFTSS_TDI_INV_OFS                        (26)                            /* !< INV Offset */
#define DFTSS_TDI_INV_MASK                       ((uint32_t)0x04000000U)         /* !< Invert digital input/output
                                                                                    relative to peripheral/GPIO */
#define DFTSS_TDI_INV_NO_CHNG                    ((uint32_t)0x00000000U)         /* !< Input and output are non-inverted */
#define DFTSS_TDI_INV_CHNG                       ((uint32_t)0x04000000U)         /* !< Input and output are inverted */
/* DFTSS_TDI[WUEN] Bits */
#define DFTSS_TDI_WUEN_OFS                       (27)                            /* !< WUEN Offset */
#define DFTSS_TDI_WUEN_MASK                      ((uint32_t)0x08000000U)         /* !< Wake up enable */
#define DFTSS_TDI_WUEN_NO_WAKEUP                 ((uint32_t)0x00000000U)         /* !< Wake up not enabled */
#define DFTSS_TDI_WUEN_WAKEUP                    ((uint32_t)0x08000000U)         /* !< Wake up enabled */
/* DFTSS_TDI[WCOMP] Bits */
#define DFTSS_TDI_WCOMP_OFS                      (28)                            /* !< WCOMP Offset */
#define DFTSS_TDI_WCOMP_MASK                     ((uint32_t)0x10000000U)         /* !< Wake up compare value */
#define DFTSS_TDI_WCOMP_MATCH_ZERO               ((uint32_t)0x00000000U)         /* !< Match 0 will wake */
#define DFTSS_TDI_WCOMP_MATCH_ONE                ((uint32_t)0x10000000U)         /* !< Match 1 will wake */

/* DFTSS_TDO Bits */
/* DFTSS_TDO[PSTATE] Bits */
#define DFTSS_TDO_PSTATE_OFS                     (6)                             /* !< PSTATE Offset */
#define DFTSS_TDO_PSTATE_MASK                    ((uint32_t)0x000000C0U)         /* !< Peripheral-Analog Channel State */
#define DFTSS_TDO_PSTATE_UNASSIGNED              ((uint32_t)0x00000000U)         /* !< P-Channel is in Unassigned State */
#define DFTSS_TDO_PSTATE_HANDOVER                ((uint32_t)0x00000040U)         /* !< P-Channel is in Handover State */
#define DFTSS_TDO_PSTATE_CONC_NOT_LOCKED         ((uint32_t)0x00000080U)         /* !< P-Channel is in Connected State and
                                                                                    not Locked (That is F field is
                                                                                    allowed to change without going back
                                                                                    through Unassigned state) */
#define DFTSS_TDO_PSTATE_CONC_LOCKED             ((uint32_t)0x000000C0U)         /* !< P-Channel is in Connected State and
                                                                                    Locked (That is F field is not
                                                                                    allowed to change to a different
                                                                                    non-Zero value until both G and P
                                                                                    channels go to Unassigned) */
/* DFTSS_TDO[GSTATE] Bits */
#define DFTSS_TDO_GSTATE_OFS                     (14)                            /* !< GSTATE Offset */
#define DFTSS_TDO_GSTATE_MASK                    ((uint32_t)0x0000C000U)         /* !< GPIO Channel State */
#define DFTSS_TDO_GSTATE_UNASSIGNED              ((uint32_t)0x00000000U)         /* !< G-Channel is in Unassigned State */
#define DFTSS_TDO_GSTATE_HANDOVER                ((uint32_t)0x00004000U)         /* !< G-Channel is in Handover State */
#define DFTSS_TDO_GSTATE_CONC_NOT_LOCKED         ((uint32_t)0x00008000U)         /* !< G-Channel is in Connected State and
                                                                                    not Locked (That is F field is
                                                                                    allowed to change without going back
                                                                                    through Unassigned state) */
#define DFTSS_TDO_GSTATE_CONC_LOCKED             ((uint32_t)0x0000C000U)         /* !< G-Channel is in Connected State and
                                                                                    Locked (That is F field is not
                                                                                    allowed to change to a different
                                                                                    non-Zero value until both G and P
                                                                                    channels go to Unassigned) */
/* DFTSS_TDO[PIPD] Bits */
#define DFTSS_TDO_PIPD_OFS                       (16)                            /* !< PIPD Offset */
#define DFTSS_TDO_PIPD_MASK                      ((uint32_t)0x00010000U)         /* !< Pull down enable */
#define DFTSS_TDO_PIPD_NO_PULL_DOWN              ((uint32_t)0x00000000U)         /* !< No pull down */
#define DFTSS_TDO_PIPD_PULL_DOWN                 ((uint32_t)0x00010000U)         /* !< Pull down */
/* DFTSS_TDO[PIPU] Bits */
#define DFTSS_TDO_PIPU_OFS                       (17)                            /* !< PIPU Offset */
#define DFTSS_TDO_PIPU_MASK                      ((uint32_t)0x00020000U)         /* !< Pull up enable */
#define DFTSS_TDO_PIPU_NO_PULL_UP                ((uint32_t)0x00000000U)         /* !< No pull up */
#define DFTSS_TDO_PIPU_PULL_UP                   ((uint32_t)0x00020000U)         /* !< Pull up */
/* DFTSS_TDO[INENA] Bits */
#define DFTSS_TDO_INENA_OFS                      (18)                            /* !< INENA Offset */
#define DFTSS_TDO_INENA_MASK                     ((uint32_t)0x00040000U)         /* !< Input enable */
#define DFTSS_TDO_INENA_INP_DISABLE              ((uint32_t)0x00000000U)         /* !< Inputs 0 to connected core */
#define DFTSS_TDO_INENA_INP_ENABLE               ((uint32_t)0x00040000U)         /* !< Inputs IO pad value to connected
                                                                                    core */
/* DFTSS_TDO[HYSTEN] Bits */
#define DFTSS_TDO_HYSTEN_OFS                     (19)                            /* !< HYSTEN Offset */
#define DFTSS_TDO_HYSTEN_MASK                    ((uint32_t)0x00080000U)         /* !< Hysteresis enable */
#define DFTSS_TDO_HYSTEN_DIS_HYST                ((uint32_t)0x00000000U)         /* !< No hysteresis */
#define DFTSS_TDO_HYSTEN_EN_HYST                 ((uint32_t)0x00080000U)         /* !< Hysteresis on */
/* DFTSS_TDO[DRV] Bits */
#define DFTSS_TDO_DRV_OFS                        (20)                            /* !< DRV Offset */
#define DFTSS_TDO_DRV_MASK                       ((uint32_t)0x00700000U)         /* !< Drive strength options */
#define DFTSS_TDO_DRV_STRENGTH_0                 ((uint32_t)0x00000000U)         /* !< Lowest drive strength */
#define DFTSS_TDO_DRV_STRENGTH_1                 ((uint32_t)0x00100000U)         /* !< Drive strength 2/8 */
#define DFTSS_TDO_DRV_STRENGTH_2                 ((uint32_t)0x00200000U)         /* !< Drive strength 3/8 */
#define DFTSS_TDO_DRV_STRENGTH_3                 ((uint32_t)0x00300000U)         /* !< Drive strength 4/8 */
#define DFTSS_TDO_DRV_STRENGTH_4                 ((uint32_t)0x00400000U)         /* !< Drive strength 5/8 */
#define DFTSS_TDO_DRV_STRENGTH_5                 ((uint32_t)0x00500000U)         /* !< Drive strength 6/8 */
#define DFTSS_TDO_DRV_STRENGTH_6                 ((uint32_t)0x00600000U)         /* !< Drive strength 7/8 */
#define DFTSS_TDO_DRV_STRENGTH_7                 ((uint32_t)0x00700000U)         /* !< Highest drive strength */
/* DFTSS_TDO[HIGHZ0] Bits */
#define DFTSS_TDO_HIGHZ0_OFS                     (24)                            /* !< HIGHZ0 Offset */
#define DFTSS_TDO_HIGHZ0_MASK                    ((uint32_t)0x01000000U)         /* !< High-Z instead of low output */
#define DFTSS_TDO_HIGHZ0_DRV_LOW                 ((uint32_t)0x00000000U)         /* !< Pin can be driven low */
#define DFTSS_TDO_HIGHZ0_DRV_HIZ                 ((uint32_t)0x01000000U)         /* !< Pin is tri-stated instead of driven
                                                                                    low */
/* DFTSS_TDO[HIGHZ1] Bits */
#define DFTSS_TDO_HIGHZ1_OFS                     (25)                            /* !< HIGHZ1 Offset */
#define DFTSS_TDO_HIGHZ1_MASK                    ((uint32_t)0x02000000U)         /* !< High-Z instead of high output */
#define DFTSS_TDO_HIGHZ1_DRV_HIGH                ((uint32_t)0x00000000U)         /* !< Pin can be driven high */
#define DFTSS_TDO_HIGHZ1_DRV_HIZ                 ((uint32_t)0x02000000U)         /* !< Pin is tri-stated instead of driven
                                                                                    high */
/* DFTSS_TDO[INV] Bits */
#define DFTSS_TDO_INV_OFS                        (26)                            /* !< INV Offset */
#define DFTSS_TDO_INV_MASK                       ((uint32_t)0x04000000U)         /* !< Invert digital input/output
                                                                                    relative to peripheral/GPIO */
#define DFTSS_TDO_INV_NO_CHNG                    ((uint32_t)0x00000000U)         /* !< Input and output are non-inverted */
#define DFTSS_TDO_INV_CHNG                       ((uint32_t)0x04000000U)         /* !< Input and output are inverted */
/* DFTSS_TDO[WUEN] Bits */
#define DFTSS_TDO_WUEN_OFS                       (27)                            /* !< WUEN Offset */
#define DFTSS_TDO_WUEN_MASK                      ((uint32_t)0x08000000U)         /* !< Wake up enable */
#define DFTSS_TDO_WUEN_NO_WAKEUP                 ((uint32_t)0x00000000U)         /* !< Wake up not enabled */
#define DFTSS_TDO_WUEN_WAKEUP                    ((uint32_t)0x08000000U)         /* !< Wake up enabled */
/* DFTSS_TDO[WCOMP] Bits */
#define DFTSS_TDO_WCOMP_OFS                      (28)                            /* !< WCOMP Offset */
#define DFTSS_TDO_WCOMP_MASK                     ((uint32_t)0x10000000U)         /* !< Wake up compare value */
#define DFTSS_TDO_WCOMP_MATCH_ZERO               ((uint32_t)0x00000000U)         /* !< Match 0 will wake */
#define DFTSS_TDO_WCOMP_MATCH_ONE                ((uint32_t)0x10000000U)         /* !< Match 1 will wake */


#ifdef __cplusplus
}
#endif

#endif /* ti_devices_msp_peripherals_hw_dftss__include */

