#include "ti_msp_dl_config.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "uart_console.h"

#define I2C_TX_PACKET_SIZE (128)
#define I2C_RX_PACKET_SIZE (128)

uint8_t gTxPacket[I2C_TX_PACKET_SIZE];
volatile uint8_t gRxPacket[I2C_TX_PACKET_SIZE];

uint8_t gExpectedRxPacket[I2C_TX_PACKET_SIZE];
/* I2C Target address */
#define I2C_TARGET_ADDRESS (0x50)
uint32_t t1 = 0, t2 = 0, counter_diff = 0;
double line_throughput = 0;

volatile bool txDmaDone = false, txDone = false;

int main(void)
{
    for(uint8_t i = 0; i < I2C_TX_PACKET_SIZE; i ++){
        gTxPacket[i] = i;
    }
    
    SYSCFG_DL_init();
    NVIC_EnableIRQ(I2C_INST_INT_IRQN);

    DL_I2CC_enableDMAEvent(
        I2C_INST, DL_I2CC_EVENT_ROUTE_2, DL_I2CC_DMA_INTERRUPT_TXFIFO_TRIGGER);

    DL_I2CC_enableInterrupt(I2C_INST, DL_I2CC_INTERRUPT_EVENT2_DMA_DONE | DL_I2CC_INTERRUPT_TX_DONE);
    

    DL_I2CC_enableDMAEvent(
        I2C_INST, DL_I2CC_EVENT_ROUTE_1, DL_I2CC_DMA_INTERRUPT_RXFIFO_TRIGGER);
    DL_SYSCTL_disableSleepOnExit();

    DL_Common_EnableCpuCycleCounter();
    while (1) {
        DL_DMA_setSrcAddr(DMA0, DMA0_CH0_CHAN_ID, (uint32_t) &gTxPacket[0]);
        DL_DMA_setDestAddr(
            DMA0, DMA0_CH0_CHAN_ID, (uint32_t)(&I2C_INST->i2cc->TXDATA));
        DL_DMA_setTransferSize(DMA0, DMA0_CH0_CHAN_ID, I2C_TX_PACKET_SIZE);
        
        DL_I2CC_resetTransfer(I2C_INST);
        DL_I2CC_startTransfer(I2C_INST, I2C_TARGET_ADDRESS, DL_I2CC_DIRECTION_TX,
        I2C_TX_PACKET_SIZE);
        t1 = DL_Common_GetCpuCycleCount();
        DL_DMA_enableChannel(DMA0, DMA0_CH0_CHAN_ID);

        while(txDone == false){
            __WFI();
        }

        counter_diff = t2 - t1;
        line_throughput = (((1.0 * ((1 + I2C_TX_PACKET_SIZE) * 8 + 2) * CPUCLK_FREQ)/counter_diff)/(1.0e6));
        uart_printf(UART_0_INST,"Throughput: %.8f Mbps\n", line_throughput);
        txDmaDone = false;
        txDone = false;
    }
}


void I2C_INST_IRQHandler(void)
{
    switch (DL_I2CC_getPendingInterrupt(I2C_INST)) {
        case DL_I2CC_IIDX_TX_DONE:
            txDone = true;
            t2 = DL_Common_GetCpuCycleCount();
            break;
        case DL_I2CC_IIDX_EVENT1_DMA_DONE:
            txDmaDone = true;
            break;
        default:
            break;
    }  
}