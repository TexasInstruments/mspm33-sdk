/*
 * Copyright (c) 2025, Texas Instruments Incorporated - http://www.ti.com
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

#include "sns_helpers.h"
#include "ti_msp_dl_config.h"

/**
 * @brief Non-Secure Callable (NSC) function - get DMA trigger routing status
 *
 * This NSC function allows non-secure code to query the DMA trigger routing
 * configuration. This is useful for diagnostics and verification.
 *
 * @return Bit field indicating DMA trigger routing status
 *         Bit 0: ADC0 trigger routed to DMA1 (1=yes, 0=no)
 *         Bit 1: UART TX trigger routed to DMA1 (1=yes, 0=no)
 */
NS_FUNC_ENTRY uint32_t get_dma_trigger_routing_status(void)
{
    uint32_t status = 0;

    /* Check ADC0 trigger routing */
    DL_GSC_DMA_Instance instance;
    instance = DL_GSC_getFixedTriggerDMAInstance(
        GSC, DL_GSC_DMA_FIXED_TRIG_ADC0_TRIG1);

    if (instance == DL_GSC_DMA_INSTANCE_1) {
        status |= (1 << 0); /* ADC0 routed to DMA1 */
    }

    /* Check UART TX trigger routing */
    instance =
        DL_GSC_getRegularTriggerDMAInstance(GSC, DL_GSC_DMA_TRIG_UC1_0_TX);

    if (instance == DL_GSC_DMA_INSTANCE_1) {
        status |= (1 << 1); /* UART TX routed to DMA1 */
    }

    return status;
}
