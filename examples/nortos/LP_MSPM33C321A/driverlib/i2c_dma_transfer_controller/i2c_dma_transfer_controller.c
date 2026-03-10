#include "ti_msp_dl_config.h"

#define I2C_TX_PACKET_SIZE (64)
#define I2C_RX_PACKET_SIZE (64)

uint8_t gTxPacket[I2C_TX_PACKET_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
    0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};
volatile uint8_t gRxPacket[I2C_TX_PACKET_SIZE];

uint8_t gExpectedRxPacket[I2C_TX_PACKET_SIZE] = {0x40, 0x41, 0x42, 0x43, 0x44,
    0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C,
    0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
    0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F};
/* I2C Target address */
#define I2C_TARGET_ADDRESS (0x50)
#ifdef __SLATE_INTERNAL__
volatile bool gDUTConfigured   = false;
volatile bool TXPacketSent     = false;
volatile bool RXPacketReceived = false;
#endif

bool dataReceived = false;
uint8_t i         = 0;
void I2C_INST_IRQHandler(void)
{
    switch (DL_I2CC_getPendingInterrupt(I2C_INST)) {
        case DL_I2CC_IIDX_EVENT2_DMA_DONE:
            dataReceived = true;
            break;
        default:
            break;
    }
}
int main(void)
{
    SYSCFG_DL_init();
#ifdef __SLATE_INTERNAL__
    /* Wait until target is configured before packet is sent */
    gDUTConfigured = true;
    __BKPT(0);
#endif

    NVIC_EnableIRQ(I2C_INST_INT_IRQN);

    DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gTxPacket[0]);
    DL_DMA_setDestAddr(
        DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(&I2C_INST->i2cc->TXDATA));
    DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, I2C_TX_PACKET_SIZE);

    DL_DMA_setSrcAddr(
        DMA0, DMA0_CH1_CHAN_ID, (uint32_t)(&I2C_INST->i2cc->RXDATA));
    DL_DMA_setDestAddr(DMA0, DMA0_CH1_CHAN_ID, (uint32_t) &gRxPacket[0]);
    DL_DMA_setTransferSize(DMA0, DMA0_CH1_CHAN_ID, I2C_RX_PACKET_SIZE);

    DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);
    DL_DMA_enableChannel(DMA0, DMA0_CH1_CHAN_ID);

    DL_I2CC_enableInterrupt(I2C_INST, DL_I2CC_INTERRUPT_EVENT1_DMA_DONE);

    DL_I2CC_enableDMAEvent(
        I2C_INST, DL_I2CC_EVENT_ROUTE_2, DL_I2CC_DMA_INTERRUPT_TXFIFO_TRIGGER);

    DL_I2CC_enableInterrupt(I2C_INST, DL_I2CC_INTERRUPT_EVENT2_DMA_DONE);

    DL_I2CC_enableDMAEvent(
        I2C_INST, DL_I2CC_EVENT_ROUTE_1, DL_I2CC_DMA_INTERRUPT_RXFIFO_TRIGGER);
    DL_SYSCTL_disableSleepOnExit();
    /*
     *
     * Fill FIFO with data. This example will send a MAX of 8 bytes since it
     * doesn't handle the case where FIFO is full
     */

    DL_I2CC_startTransfer(I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_TX,
        I2C_TX_PACKET_SIZE);

    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE)) {
    }

    DL_I2CC_resetTransfer(I2C_INST);
    DL_I2CC_startTransfer(I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_RX,
        I2C_RX_PACKET_SIZE);

    while (!(DL_I2CC_getStatus(I2C_INST) & DL_I2CC_STATUS_IDLE)) {
    }

    while (dataReceived == false) {
        __WFE();
    }

    int allCorrect = 1;
    for (int i = 0; i < I2C_RX_PACKET_SIZE; i++) {
        if (gExpectedRxPacket[i] != gRxPacket[i]) allCorrect = 0;
    }

    if (allCorrect == 1) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } 
    else{
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }

    __BKPT(0);
}
