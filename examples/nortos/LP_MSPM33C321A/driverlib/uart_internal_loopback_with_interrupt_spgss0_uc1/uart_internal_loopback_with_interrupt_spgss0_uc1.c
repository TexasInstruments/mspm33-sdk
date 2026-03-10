#include "ti_msp_dl_config.h"

/* Delay for 5ms to ensure UART TX is idle before starting transmission */
#define UART_TX_DELAY (160000)

uint8_t txData = 0, rxData = 0;
bool dataReceived;

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_IIDX_RX:
            rxData       = DL_UART_receiveDataBlocking(UART_0_INST);
            dataReceived = true;
            while (rxData != txData) {
            };
            break;
        default:
            break;
    }
}

int main(void)
{
    SYSCFG_DL_init();
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    dataReceived = false;
    txData       = 5;
    DL_UART_transmitData(UART_0_INST, txData);

    while (dataReceived == false) {
        __WFE();
    }
    if (txData == rxData) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } 
    else{
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    __BKPT(0);
}
