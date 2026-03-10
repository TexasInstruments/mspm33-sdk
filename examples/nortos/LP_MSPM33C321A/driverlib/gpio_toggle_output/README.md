## Example Summary

This example demonstrates how to toggle GPIO pins using the hardware toggle register. Three GPIO pins connected to the RGB LED on the LaunchPad are configured as digital outputs and toggled in a pattern.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA2 | Standard Output (LED1 Blue) |
| GPIOC | PC26 | Standard Output (LED2 Red) |
| GPIOC | PC27 | Standard Output (LED3 Green) |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA2 | GPIOA | USER_LED_1 | N/A | <ul><li>PA2 is connected to LED1 Blue<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | USER_LED_2 | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC27 | GPIOC | USER_LED_3 | N/A | <ul><li>PC27 is connected to LED3 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
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

Compile, load and run the example.
RGB LEDs will toggle with red being opposite of blue and green.

USER_TEST_PIN GPIO will mimic the behavior of the LED1 and LED3 pins on the
BoosterPack header and can be used to verify the LED behavior
