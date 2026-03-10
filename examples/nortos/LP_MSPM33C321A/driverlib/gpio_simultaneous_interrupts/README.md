## Example Summary

This example demonstrates how to simultaneously service multiple GPIO interrupts from different GPIO ports. Three input pins are configured with interrupts, and when an interrupt occurs, the corresponding LED is toggled. The example shows how to handle interrupts from multiple GPIO ports (GPIOA and GPIOB) and how to determine which pin triggered the interrupt.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output (Red LED) |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOA | PA2 | Standard Output (Blue LED) |
| GPIOA | PA12 | Standard Input with interrupt |
| GPIOA | PA13 | Standard Input with interrupt |
| GPIOB | PB15 | Standard Input with interrupt |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | USER_LED_1 | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC27 | GPIOC | USER_LED_2 | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA2 | GPIOA | USER_LED_3 | N/A | <ul><li>PA2 is connected to LED2 Blue<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA12 | GPIOA | USER_SWITCH_1 | N/A | <ul><li>PA12 is configured as input with rising edge interrupt</ul> |
| PA13 | GPIOA | USER_SWITCH_2 | N/A | <ul><li>PA13 is configured as input with rising edge interrupt</ul> |
| PB15 | GPIOB | USER_SWITCH_3 | N/A | <ul><li>PB15 is configured as input with rising edge interrupt</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage

The example performs the following operations:
1. Initializes three GPIO pins as digital outputs connected to the RGB LED (red, green, and blue)
2. Initializes three GPIO pins as digital inputs with rising edge interrupts
3. Enables interrupts for GPIOA and GPIOB in the NVIC
4. Enters a low-power mode using wait-for-interrupt

When an interrupt occurs:
- If USER_SWITCH_1 (PA12) triggers an interrupt, the red LED (PC26) is toggled
- If USER_SWITCH_2 (PA13) triggers an interrupt, the green LED (PC27) is toggled
- If USER_SWITCH_3 (PB15) triggers an interrupt, the blue LED (PA2) is toggled

The example demonstrates how to:
1. Configure multiple GPIO pins for interrupt detection
2. Handle interrupts from different GPIO ports
3. Determine which pin triggered the interrupt
4. Service multiple interrupts that may occur simultaneously

Compile, load and run the example. Connect the input pins to signal sources to trigger the interrupts and observe the LEDs toggling in response. For demonstration purposes, you can connect the input pins to the same signal source to trigger multiple interrupts simultaneously.
