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

#include "config.h"
#include "partition.h"
#include "sns_helpers.h"
#include "ti_msp_dl_config.h"

void configure_sau( void )
{
    /* Disable SAU */
    SAU->CTRL = 0;

    TZ_SAU_Setup( );

    /* Enable SAU */
    SAU->CTRL = SAU_CTRL_ENABLE_Msk;
}

void soc_configure( )
{
    DL_GSC_setVTOR_NS( GSC, NON_SECURE_APP0_START );
    __TZ_set_MSP_NS( NON_SECURE_APP0_SRAM_START + NON_SECURE_APP0_SRAM_LEN );

    /*
     *  Configure SRAM Banks as per need. This should come from sys config.
     */

    GSC->SPC_SECATTRIB0 = 0xFFFFFFFF; // SRAM Bank0 entirely secure
    GSC->SPC_SECATTRIB1 = 0x00000000; // Bank1 non secure

    // GSC->SPC_SECATTRIB2 = APPS_GSC_PROPS_SRAM_BANK2;
    // GSC->SPC_SECATTRIB3 = APPS_GSC_PROPS_SRAM_BANK3;

    /*
     *  Configure Flash Banks as per need. This should come from sys config.
     */
    GSC->FPC_SECATTRIBA0 = 0xFFFFFFFF;
    //
    GSC->FPC_SECATTRIBB0 = 0x0; // calc properly and configure for only non secure

    // Configure Flash Bank1
    // GSC->FPC_SECATTRIBA1 = APPS_GSC_PROPS_FLASH_BANK_A1;
    // GSC->FPC_SECATTRIBB1 = APPS_GSC_PROPS_FLASH_BANK_B1;

    configure_sau( );

    __ISB( );
    __DSB( );
}

int main( void )
{
    SYSCFG_DL_init( );

    soc_configure( );

    const uint32_t *p_ns_vector_table = ( uint32_t * ) NON_SECURE_APP0_START;
    const uint32_t *p_ns_reset_address =
        ( const uint32_t * ) ( ( uint32_t ) NON_SECURE_APP0_START + sizeof( uint32_t ) );
    nonsecure_func_t p_ns_reset = ( nonsecure_func_t ) ( *p_ns_reset_address );
    p_ns_reset( );

    while ( 1 )
    {
    }
}
