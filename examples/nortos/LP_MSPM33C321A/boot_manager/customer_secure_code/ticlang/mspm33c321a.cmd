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
-uinterruptVectors
--stack_size=16384

/* TODO: remove copies of the defines as config */
#define CSC_SECRET_ADDR 0x7000
#define CSC_SECRET_SIZE  0x400
#define CSC_LOCK_STORAGE_ADDR   0x7400
#define CSC_LOCK_STORAGE_SIZE   0x400


//#include "customer_secure_config.h"



/* Region of Flash that is read-protected after INITDONE */
#define FLASH_SIZE    0x7000

MEMORY
{
  FLASH 			  (RX)  : origin = 0x10000000, length = FLASH_SIZE, fill = 0xFFFFFFFF,
  SECRET			  (RX)  : origin = end(FLASH), length = CSC_SECRET_SIZE, fill = 0xFFFFFFFF,
  LOCK_STORAGE	(RX)  : origin = end(SECRET), length = CSC_LOCK_STORAGE_SIZE, fill = 0xFFFFFFFF,
  BCR_CONFIG      (R)   : origin = 0x80101800, length = 0x00000400,
  BSL_CONFIG      (R)   : origin = 0x80101C00, length = 0x00000400,
	SRAM            (RWX) : origin = 0x30000000, length = 0x00040000,

}

SECTIONS
{
    .intvecs:  > 0x10000000
    .text   : palign(16) {} > FLASH
    .const  : palign(16) {} > FLASH
    .cinit  : palign(16) {} > FLASH
    .pinit  : palign(16) {} > FLASH
    .rodata : palign(16) {} > FLASH

    .ARM.exidx    :  palign(16)  {} > FLASH
    .init_array   :  palign(16)  {} > FLASH
    .binit        : palign(16) {} > FLASH
    .TI.ramfunc   : load = FLASH, palign(16), run=SRAM, table(BINIT)

	  .secret  : palign(16) {} > SECRET
    .lockStg : (NOINIT) palign(16) {} > LOCK_STORAGE

    .vtable :   > SRAM
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM
    .stack  :   > SRAM (HIGH)

    .BCRConfig  : {} > BCR_CONFIG
    .BSLConfig  : {} > BSL_CONFIG

}
