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

#ifndef __CONFIG_TZ_DMA_APP__
#define __CONFIG_TZ_DMA_APP__

/* Non-Secure Application Memory Configuration */
#define NON_SECURE_APP0_START 0x10000U /* Non-secure app start address */
#define NON_SECURE_APP0_LEN 0xF0000U   /* Non-secure app length (960KB) */

/* Secure Non-Secure Callable (NSC) Region Configuration */
#define SECURE_NSC_APP0_START 0xF800U /* NSC region start address */
#define SECURE_NSC_APP0_LEN 0x800U    /* NSC region length (2KB) */

/* Non-Secure SRAM Configuration (Pages 4-15: 192KB) */
#define NON_SECURE_APP0_SRAM_START 0x20010000U /* Non-secure SRAM start */
#define NON_SECURE_APP0_SRAM_LEN 0x30000U /* Non-secure SRAM length (192KB) */

/* DMA Configuration */
#define DMA_FILL_SIZE_WORDS 16  /* Size of secure DMA buffer */
#define NS_BUFFER_SIZE 16       /* Size of non-secure DMA buffer */
#define FILL_PATTERN 0xA5A5A5A5 /* Pattern for DMA fill operation */

/* Delay Configuration */
#define TRANSITION_DELAY_CYCLES 2000000 /* Delay before transitioning to NS */
#define TEST_DELAY_CYCLES 1000000       /* Delay between test operations */

#endif /* __CONFIG_TZ_DMA_APP__ */
