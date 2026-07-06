## Example Summary

This example demonstrates how to use C++ to control GPIO pins. It creates a C++ class that encapsulates GPIO operations and uses it to toggle LEDs and a test pin.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA2 | Standard Output (Blue LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOB | PB6 | Standard Output (Test Pin) |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA2 | GPIOA | USER_LED_1 | N/A | <ul><li>PA2 is connected to LED2 Blue<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | USER_LED_2 | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC27 | GPIOC | USER_LED_3 | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PB6 | GPIOB | USER_TEST | N/A | <ul><li>PB6 is used as a test pin<br><ul><li>This pin can be monitored to verify the LED behavior</ul></ul> |
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

The example demonstrates how to use C++ to control GPIO pins by:
1. Setting the initial state of the pins:
   - LED1 (Blue), LED3 (Green), and USER_TEST are set to ON
   - LED2 (Red) is set to OFF
2. In an infinite loop:
   - Waiting for approximately 1 second
   - Toggling all four pins

This creates a pattern where the blue and green LEDs toggle in sync, while the red LED toggles in the opposite pattern. The USER_TEST pin mimics the behavior of the blue and green LEDs and can be monitored with an oscilloscope or logic analyzer to verify the LED behavior.

Compile, load and run the example. The RGB LED will toggle with red being opposite of blue and green. The USER_TEST pin (PB6) will mimic the behavior of the blue and green LEDs.
