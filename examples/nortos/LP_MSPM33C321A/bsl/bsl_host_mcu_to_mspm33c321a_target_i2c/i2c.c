#include <bsl_i2c.h>
#include <i2c.h>
#include "stdint.h"
#include "ti_msp_dl_config.h"

uint8_t test_d;
uint8_t I2C_writeBuffer(uint8_t *pData, uint32_t ui32Cnt)
{
    uint8_t gTxCount;

    gTxCount             = DL_I2CC_fillTXFIFO(I2C_INST, pData, ui32Cnt);
    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE))
        ;

    DL_I2CC_startTransfer(
        I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_TX, ui32Cnt);

    DL_Common_updateReg(&I2C_INST->i2cc->CTR, UNICOMMI2CC_CTR_FRM_START_ENABLE,
        UNICOMMI2CC_CTR_FRM_START_MASK);

    while (gTxCount < ui32Cnt) {
        while (DL_I2CC_getRawInterruptStatus(
                   I2C_INST, DL_I2CC_INTERRUPT_TXFIFO_EMPTY) == 0)
            ;
        gTxCount +=
            DL_I2CC_fillTXFIFO(I2C_INST, pData + gTxCount, ui32Cnt - gTxCount);
    }
    /* Poll until the Controller writes all bytes */
    while (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_BUSY_BUS)
        ;
    /* Trap if there was an error */
    if (DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_ERROR) {
        /* LED will remain high if there is an error */
        return 1;
    }
    // res = DL_UART_receiveDataBlocking(UART_0_INST);
    return 0;
}

void I2C_readBuffer(uint8_t *pData, uint32_t ui32Cnt)
{
    /* Wait for I2C to be Idle */
    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE))
        ;
    DL_I2CC_resetTransfer(I2C_INST);
    delay_cycles(100);
    /* Send a read request to Target */
    DL_I2CC_startTransfer(
        I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_RX, ui32Cnt);

    DL_Common_updateReg(&I2C_INST->i2cc->CTR, UNICOMMI2CC_CTR_FRM_START_ENABLE,
        UNICOMMI2CC_CTR_FRM_START_MASK);

    /*
     * Receive all bytes from target. LED will remain high if not all bytes
     * are received
     */
    delay_cycles(100);
    // while (!(DL_UNICOMMI2CC_getStatus(I2C_INST) & DL_UNICOMMI2CC_STATUS_IDLE)){}

    for (uint32_t i = 0; i < ui32Cnt; i++) {
        while (DL_I2CC_isRXFIFOEmpty(I2C_INST))
            ;
        *pData = DL_I2CC_receiveData(I2C_INST);
        pData++;
    }
}

//uint8_t test_d;

uint8_t Status_check(void)
{
    i2c_error_t i2c_ack;
    uint8_t buf[1] = {0xBB};
    i2c_ack        = I2C_writeBuffer(buf, 1);
    I2C_readBuffer(BSL_RX_buffer, 1);
    return BSL_RX_buffer[0];
}
