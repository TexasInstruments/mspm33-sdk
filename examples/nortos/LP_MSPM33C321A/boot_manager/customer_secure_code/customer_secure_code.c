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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <ti/driverlib/dl_flashctl.h>

#include "bootutil/bootutil.h"
#include "bootutil/image.h"
#include "bootutil/security_cnt.h"
#include "customer_secure_config.h"
#include "keystore.h"
#include "lockable_storage_private.h"
#include "mcuboot_config/mcuboot_logging.h"
#include "rollback.h"
#include "secret.h"
#include "ti_msp_dl_config.h"

/* sets the range over the lockable storage. Includes secret */
#define LOCKABLE_FLASH_FIREWALL 0x00030000

/* GSC HDP flash protection constants.
 * These are local to customer_secure_code.c; customer_secure_config.h is
 * managed as a sysconfig_src in the build so additions there are not picked up.
 */
#define FLASH_SECTOR_SIZE 0x800
#define CSC_FLASH_BANK 0
#define CSC_CODE_BASE_ADDR 0x0000
#define CSC_FLASH_SIZE 0x7000

#define BOOT_PRIMARY_SLOT 0
#define BOOT_SECONDARY_SLOT 1

/* current debug image settings. Image could obviously be much larger */
#define PRIMARY_SLOT_OFFSET (CSC_APPLICATION_IMAGE1_BASE_ADDR)
#define SECONDARY_SLOT_OFFSET (CSC_NS_APPLICATION_IMAGE2_BASE_ADDR)
#define IMG_SLOT_SIZE (CSC_APPLICATION_IMAGE1_SIZE)

/* do_boot is called in the event of a successful verification of an image */
static void do_boot(struct boot_rsp *rsp)
{
    MCUBOOT_LOG_INF("Starting Main Application");
    MCUBOOT_LOG_INF("  Image Start Offset: 0x%x", (int) rsp->br_image_off);

    uint32_t vector_table =
        0x0 + PRIMARY_SLOT_OFFSET + rsp->br_hdr->ih_hdr_size;

    MCUBOOT_LOG_INF("  Vector Table Start Address: 0x%x", (int) vector_table);

    GSC->VTOR_S = (uint32_t) vector_table;
}

volatile bool digestReady = false;
volatile bool dmaDone     = false;

__attribute__((interrupt)) void SHAW_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(SHAW_INT_IRQn);

    if (DL_SHAW_getOutputReadyInterruptStatus(SHAW_INST)) {
        DL_SHAW_clearOutputReadyInterrupt(SHAW_INST);

        digestReady = true;
    }
}

__attribute__((interrupt)) void DMA0_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(DMA0_INT_IRQn);

    if (DL_DMA_getRawInterruptStatus(DMA0, DL_DMA_INTERRUPT_CHANNEL0)) {
        DL_DMA_clearInterruptStatus(DMA0, DMA0_CH0_CHAN_ID);

        dmaDone = true;
    }
}

/* Fail procedure for mcuboot if no valid image found. Currently just toggles
 * the red LED.
 */
void mcubootFail(void)
{
    while (1) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);

        /* Stop for debug */
        __BKPT(0);
        delay_cycles(10000000);
    }
}

int main(void)
{
    fih_int bootStatus;
    struct boot_rsp bootRsp;
    __enable_irq();
    SYSCFG_DL_init();

    if (!DL_SYSCTL_isINITDONEIssued()) {
        trace_init();
        MCUBOOT_LOG_INF("mcuboot_app");

        boot_return_highest_version(&bootRsp);

#ifdef CSC_ENABLE_ROLLBACK_PROTECTION
        /* Check that the current version is at least as recent, if not, fail.
         * TODO: this is a preliminary version of rollback protection, more secure monotonic
         * variants will be explored in future releases of the Customer Secure Code, and
         * being integreated into MCUBoot.
         */
        Rollback_status status;
        status = Rollback_compareCount(bootRsp.br_hdr->ih_ver.iv_major);
        if (status != ROLLBACK_OK) {
            mcubootFail();
        }
#endif
        bootStatus = boot_go(&bootRsp);

        if ((0 == bootStatus) && (IMAGE_MAGIC == bootRsp.br_hdr->ih_magic)) {
            MCUBOOT_LOG_INF("bootRsp: slot = %x, offset = %x, ver=%d.%d.%d.%d",
                bootStatus, bootRsp.br_image_off,
                bootRsp.br_hdr->ih_ver.iv_major,
                bootRsp.br_hdr->ih_ver.iv_minor,
                bootRsp.br_hdr->ih_ver.iv_revision,
                bootRsp.br_hdr->ih_ver.iv_build_num);

#ifdef CSC_ENABLE_ROLLBACK_PROTECTION
            Rollback_storeCount(bootRsp.br_hdr->ih_ver.iv_major);
#endif

        } else {
            mcubootFail();
        }

        if (bootRsp.br_image_off != PRIMARY_SLOT_OFFSET) {
            DL_SYSCTL_executeFromUpperFlashBank();
        } else {
            DL_SYSCTL_executeFromLowerFlashBank();
        }

        do_boot(&bootRsp);

        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);

#ifdef CSC_ENABLE_KEYSTORE
        /* Transfer the static AES keys from .secret flash into the hardware
         *  AES KEYSTORE.
         */
        Keystore_storeKeys();
#endif

        /* Apply GSC Hide Protection(HDP) on the entire CSC flash region
         */
        DL_GSC_enableHDPMode(GSC);
        DL_GSC_setHDPAccess(GSC, false, CSC_FLASH_BANK);
        /* Configure to protect the CSC code region
         * from CSC_CODE_BASE_ADDR to CSC_CODE_BASE_ADDR + CSC_FLASH_SIZE.
         */
        DL_GSC_configureHDPRegion(GSC, CSC_CODE_BASE_ADDR / FLASH_SECTOR_SIZE,
            (CSC_CODE_BASE_ADDR + CSC_FLASH_SIZE) / FLASH_SECTOR_SIZE,
            CSC_FLASH_BANK);
        /* Configure to protect .secret and .lockStg
         */
        DL_GSC_configureHDPRegion(GSC, CSC_SECRET_ADDR / FLASH_SECTOR_SIZE,
            (CSC_LOCK_STORAGE_ADDR + CSC_LOCK_STORAGE_SIZE) /
                FLASH_SECTOR_SIZE,
            CSC_FLASH_BANK);

        DL_SYSCTL_issueINITDONE();
    }
    mcubootFail();
}
