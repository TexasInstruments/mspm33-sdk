#include "ti_msp_dl_config.h"

#define I2C_RX_PACKET_SIZE (128)
#define I2C_TX_PACKET_SIZE (128)

uint8_t gExpectedRxPacket[I2C_RX_PACKET_SIZE];
uint8_t gTxPacket[I2C_RX_PACKET_SIZE];
volatile uint8_t gRxPacket[I2C_RX_PACKET_SIZE];

/* I2C  address */
#define I2C_TARGET_ADDRESS (0x50)

volatile bool dataReceived = false;
volatile bool rxDmaDone = false;

int main(void)
{
    for(uint32_t i = 0; i < I2C_RX_PACKET_SIZE; i ++){
        gExpectedRxPacket[i] = i;
    }
    SYSCFG_DL_init();
    NVIC_EnableIRQ(I2C_INST_INT_IRQN);

    DL_I2CT_enableInterrupt(I2C_INST, DL_I2CT_INTERRUPT_EVENT1_DMA_DONE);

    DL_I2CT_enableDMAEvent(
        I2C_INST, DL_I2CT_EVENT_ROUTE_2, DL_I2CT_DMA_INTERRUPT_TXFIFO_TRIGGER);

    DL_I2CT_enableInterrupt(I2C_INST, DL_I2CT_INTERRUPT_EVENT2_DMA_DONE);
    DL_I2CT_enableDMAEvent(
        I2C_INST, DL_I2CT_EVENT_ROUTE_1, DL_I2CT_DMA_INTERRUPT_RXFIFO_TRIGGER);



    DL_SYSCTL_disableSleepOnExit();


    while (1) {
        DL_DMA_setSrcAddr(
            DMA0, DMA0_CH1_CHAN_ID, (uint32_t)(&I2C_INST->i2ct->RXDATA));
        DL_DMA_setDestAddr(DMA0, DMA0_CH1_CHAN_ID, (uint32_t) &gRxPacket[0]);
        DL_DMA_setTransferSize(DMA0, DMA0_CH1_CHAN_ID, I2C_RX_PACKET_SIZE);
        DL_DMA_enableChannel(DMA0, DMA0_CH1_CHAN_ID);
        while(rxDmaDone == false){
            __WFI();
        }

        rxDmaDone = false;

    }
}

void I2C_INST_IRQHandler(void)
{
    switch (DL_I2CT_getPendingInterrupt(I2C_INST)) {
        case DL_I2CT_IIDX_EVENT2_DMA_DONE:
            rxDmaDone = true;
            break;
        default:
            break;
    }  
}
