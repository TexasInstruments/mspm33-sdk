#include "ti_msp_dl_config.h"
bool gActivateSleepPolicy = false;
bool gTogglePolicy        = false;
uint8_t txData = 5, rxData = 0;
int main(void)
{
    SYSCFG_DL_init();
    DL_UART_transmitData(UART_0_INST, txData);
    rxData = DL_UART_receiveDataBlocking(UART_0_INST);
    if (txData == rxData) {
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_PASS_LED_PIN);
    } 
    else{
        DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_FAIL_LED_PIN);
    }
    __BKPT(0);
    return 0;
}
