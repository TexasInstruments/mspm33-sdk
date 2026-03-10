#include "uart.h"
#include "stdint.h"
#include "ti_msp_dl_config.h"

uint8_t test_d;
uint8_t UART_writeBuffer(uint8_t *pData, uint32_t ui32Cnt)
{
    uint8_t res;

    while (ui32Cnt--) {
        DL_UART_transmitDataBlocking(UART_0_INST, *pData);
        pData++;
        // delay_cycles(10000);
    }
    res = DL_UART_receiveDataBlocking(UART_0_INST);
    return res;
}

void UART_readBuffer(uint8_t *pData, uint32_t ui32Cnt)
{
    //   uint8_t res;
    while (ui32Cnt-- > 0) {
        *pData = DL_UART_receiveDataBlocking(UART_0_INST);
        pData++;
    }
}

//uint8_t test_d;

uint8_t Status_check(void)
{
    uint8_t res;
    /* Wait until all bytes have been transmitted and the TX FIFO is empty */
    while (DL_UART_isBusy(UART_0_INST))
        ;
    DL_UART_transmitDataBlocking(UART_0_INST, 0xBB);
    /* Wait until all bytes have been transmitted and the TX FIFO is empty */
    while (DL_UART_isBusy(UART_0_INST))
        ;
    res = DL_UART_receiveDataBlocking(UART_0_INST);
    return res;
}
