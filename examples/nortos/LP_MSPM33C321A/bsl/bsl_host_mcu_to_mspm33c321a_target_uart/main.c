/*
 * Copyright (c) 2026, Texas Instruments Incorporated
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

#include <ti_msp_dl_config.h>

#ifdef I2C_Plugin
#include <application_image_i2c.h>
#include <bsl_i2c.h>
#include "i2c.h"
#endif
#ifdef UART_Plugin
#include <application_image_uart.h>
#include "bsl_uart.h"
#include "uart.h"
#endif

void ToggleLeds(void);

BSL_error_t bsl_err;
uint8_t status;
/*=============================================================================
 * Here is password of the boot code for update. The last two bytes if the start address of the boot code.
 */
const uint8_t BSL_PW_RESET[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF};

int main(void)
{
    uint8_t section;

    SCB->VTOR = 0x30000001;
    SYSCFG_DL_init();
    ToggleLeds(); /* TO show code start */

    while (1) {
        bsl_err = Host_BSL_Connection();
        delay_cycles(100000);
        bsl_err = Host_BSL_GetID();

        if (bsl_err == eBSL_success) {
            bsl_err = Host_BSL_loadPassword((uint8_t*) BSL_PW_RESET);
            if (bsl_err == eBSL_success) {
                bsl_err = Host_BSL_MassErase();
                if (bsl_err == eBSL_success) {
                    delay_cycles(DELAY_BSL_OP);
                    /* WRITE THE ENTIRE PROGRAM MEMORY SECTION TO TARGET */
                    for (section = 0;
                         section < (sizeof(App2_Addr) / sizeof(App2_Addr[0]));
                         section++) {
                        bsl_err = Host_BSL_writeMemory(App2_Addr[section],
                            App2_Ptr[section], App2_Size[section]);
                        if (bsl_err != eBSL_success) break;
                    }
                    /* Start the application */
                    Host_BSL_StartApp();
                } else {
                    /* Mass Erase failed error */
                    TurnOnErrorLED();
                    __BKPT(0);
                }
            } else {
                /* Password failed error */
                TurnOnErrorLED();
                __BKPT(0);
            }
        } else {
            /* connection failed error */
            TurnOnErrorLED();
            __BKPT(0);
        }
    }
}

/*
 * Toggle the LEDs to show we are going to BSL the target
 */
void ToggleLeds(void)
{
    DL_GPIO_clearPins(GPIO_LED_PA0_PORT, GPIO_LED_PA0_PIN);
    DL_GPIO_setPins(GPIO_LED_Error_PORT, GPIO_LED_Error_PIN);
    delay_cycles(1600000);
    DL_GPIO_setPins(GPIO_LED_PA0_PORT, GPIO_LED_PA0_PIN);
    DL_GPIO_clearPins(GPIO_LED_Error_PORT, GPIO_LED_Error_PIN);
}
