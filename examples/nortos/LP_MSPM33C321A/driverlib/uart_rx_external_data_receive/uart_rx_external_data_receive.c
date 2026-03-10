#include "ti_msp_dl_config.h"

#define UART_TX_DELAY (160000)
#define transfer_size 64
uint8_t txData[transfer_size] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
    0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,
    0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F},
        rxData[transfer_size];
bool dataReceived;
uint8_t i = 0;
void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_IIDX_RX:
            while (i < transfer_size) {
                while (DL_UART_isRXFIFOEmpty(UART_0_INST)) {
                }
                rxData[i] = DL_UART_receiveData(UART_0_INST);
                i         = i + 1;
            }
            dataReceived = true;
            break;
        default:
            break;
    }
}

int main(void)
{
    SYSCFG_DL_init();
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    DL_UART_enableInterrupt(UART_0_INST, DL_UART_INTERRUPT_RX);
    dataReceived = false;

    while (dataReceived == false) {
        __WFE();
    }
    int allCorrect = 1;
    for (int i = 0; i < transfer_size; i++) {
        if (txData[i] != rxData[i]) allCorrect = 0;
    }

    if (allCorrect == 1) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } 
    else{
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    __BKPT(0);
}
