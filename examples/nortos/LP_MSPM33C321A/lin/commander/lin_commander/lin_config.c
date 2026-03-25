/*
 * Copyright (c) 2026, Texas Instruments Incorporated - http://www.ti.com
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

#include "lin_config.h"
#include <stdio.h>

uint8_t gMessageTableIndex = 0;
LIN_RX_STATE LIN_state = LIN_RX_STATE_BREAK;
uint8_t byteCounter   = 0;
LIN_word_t tempChksum;



#if defined(__MCU_HAS_UNICOMMUART__)

void LIN_Commander_sendPID(UNICOMM_Inst_Regs *unicomm, uint8_t tableIndex, uint8_t *TXmsgBuffer, LIN_table_record_t *messageTable)
{
    /* Enable LIN message reception */
    DL_UART_enableInterrupt(unicomm, DL_UART_INTERRUPT_RX);

    /* Transmit BREAK, SYNC byte, and PID */
    DL_UART_enableLINSendBreak(unicomm);
    delay_cycles(LIN_BREAK_LENGTH); /* Set break for ~1ms */
    DL_UART_disableLINSendBreak(unicomm);
    DL_UART_transmitDataBlocking(unicomm, LIN_SYNC_BYTE);
    DL_UART_transmitDataBlocking(unicomm, messageTable[tableIndex].msgID);
    while(DL_UART_isBusy(unicomm));

    /* Keep track of which message PID we are using */
    gMessageTableIndex = tableIndex;

    /*
     * If a callback function is present corresponding to the message PID that
     * was transmitted, then we do not transmit data, but expect to receive
     * data back from the Responder.
     */
    if (messageTable[tableIndex].callbackFunction != NULL) {
        /*Enable Timer to detect timeout issue */
        DL_Timer_setLoadValue(TIMER_0_INST, TIMEOUT);
        DL_Timer_startCounter(TIMER_0_INST);
    }

    else {
        /*
         * There is no callback associated with the message PID, so we
         * transmit a data packet to the Responder
         */

#ifdef Transmit_INT 
            DL_UART_clearInterruptStatus(unicomm, DL_UART_INTERRUPT_TX);
            DL_UART_enableInterrupt(unicomm, DL_UART_INTERRUPT_TX);
            DL_UART_transmitData(unicomm, TXmsgBuffer[0]);

#else
            LIN_Commander_transmitMessage(unicomm, TXmsgBuffer, messageTable);
#endif
    }
}


void LIN_Commander_receiveMessage(UNICOMM_Inst_Regs *unicomm, uint8_t rxByte, uint8_t *msgBuffer, LIN_table_record_t *messageTable)
{
    uint8_t checksum;
    uint8_t rxChecksum;
    LIN_function_ptr_t callbackFunction;

    switch (LIN_state) {
        case LIN_RX_STATE_DATA:
            /* Save the received byte to the RX message buffer */
            msgBuffer[byteCounter] = rxByte;
            /* Use received byte to add to checksum calculation */
            tempChksum.word += msgBuffer[byteCounter];

            /*
            * Check if the number of received bytes is greater than the expected
            * number of data bytes. If yes, then the checksum byte was just
            * just received.
            */
            byteCounter++;
            if (byteCounter >= messageTable[gMessageTableIndex].msgSize) {
                LIN_state = LIN_RX_STATE_CHECKSUM;
            }
            DL_Timer_startCounter(TIMER_0_INST);
            break;

        case LIN_RX_STATE_CHECKSUM:
            rxChecksum = rxByte;
            DL_UART_disableInterrupt(unicomm, DL_UART_INTERRUPT_RX);

            /* Calculate the checksum based on the received data */
            tempChksum.word += messageTable[gMessageTableIndex].msgID;
            tempChksum.word = tempChksum.byte[0] + tempChksum.byte[1];
            checksum        = tempChksum.byte[0];
            checksum += tempChksum.byte[1];
            checksum = 0xFF - checksum;

            /* Reset the data */
            byteCounter     = 0;
            tempChksum.word = 0;
            LIN_state       = LIN_RX_STATE_BREAK;

            /* Compare the received checksum to the calculated checksum */
            if (rxChecksum == checksum) {
                /* If they match, call the corresponding callback function */
                callbackFunction =
                    messageTable[gMessageTableIndex].callbackFunction;
                callbackFunction();
            }

            /* Reset the data */
            byteCounter     = 0;
            tempChksum.word = 0;
            LIN_state       = LIN_RX_STATE_DATA;
            break;

        case LIN_RX_STATE_BREAK:
            LIN_state = LIN_RX_STATE_SYNC;
            break;
        case LIN_RX_STATE_SYNC:
            LIN_state = LIN_RX_STATE_ID;
            break;
        case LIN_RX_STATE_ID:
            LIN_state = LIN_RX_STATE_DATA;
            break;
        default:
            break;
    }
}

/*
 * If Transmit_INT  : When this function is called once, one data will be sent.
 * else this function will send all of Tx package
 */
void LIN_Commander_transmitMessage(UNICOMM_Inst_Regs *unicomm, uint8_t *msgBuffer, LIN_table_record_t *messageTable)
{
    uint8_t checksum;

#ifdef Transmit_INT

    static uint8_t locIndex = 1;
    static LIN_word_t tempChksum_TX;

    tempChksum_TX.word += msgBuffer[locIndex];
    DL_UART_transmitData(unicomm, msgBuffer[locIndex++]);

    if(locIndex >= messageTable[gMessageTableIndex].msgSize)
    {

        /* Calculate and then transmit the checksum*/
        tempChksum_TX.word = tempChksum_TX.word + messageTable[gMessageTableIndex].msgID + msgBuffer[0];
        tempChksum_TX.word = tempChksum_TX.byte[0] + tempChksum_TX.byte[1];

        checksum = tempChksum_TX.byte[0];
        checksum += tempChksum_TX.byte[1];
        checksum = 0xFF - checksum;
        DL_UART_transmitDataBlocking(unicomm, checksum);
        DL_UART_disableInterrupt(unicomm, DL_UART_INTERRUPT_TX);
        /* Toggle LED1 with TX packet */
        DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        /* Clear RXFIFO */
        while (!DL_UART_isRXFIFOEmpty(unicomm)) {
            DL_UART_receiveData(unicomm);
        }

        /*Reset state*/
        locIndex = 1;
        tempChksum_TX.word = 0;
        while (DL_UART_isBusy(unicomm));
        

    }


#else
    uint8_t locIndex;
    LIN_word_t tempChksum_TX;

    tempChksum_TX.word = 0;

        for (locIndex = 0; locIndex < messageTable[gMessageTableIndex].msgSize;locIndex++)
        {
            DL_UART_transmitDataBlocking(unicomm, msgBuffer[locIndex]);
            tempChksum_TX.word += msgBuffer[locIndex];
        }

        /* Calculate and then transmit the checksum*/
        tempChksum_TX.word += messageTable[gMessageTableIndex].msgID;
        tempChksum_TX.word = tempChksum_TX.byte[0] + tempChksum_TX.byte[1];

        checksum = tempChksum_TX.byte[0];
        checksum += tempChksum_TX.byte[1];
        checksum = 0xFF - checksum;

        DL_UART_transmitDataBlocking(unicomm, checksum);
        /* Toggle LED1 with TX packet */
        DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        
        while (!DL_UART_isRXFIFOEmpty(unicomm)) {
            DL_UART_receiveData(unicomm);
        }

        while (DL_UART_isBusy(unicomm));
#endif
}
#endif


void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
            LIN_state = LIN_RX_STATE_BREAK;
            byteCounter     = 0;
            tempChksum.word = 0;
            break;
        default:
            break;
    }
}

