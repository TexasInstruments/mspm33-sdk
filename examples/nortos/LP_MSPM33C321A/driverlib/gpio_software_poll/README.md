## Example Summary

This example demonstrates how to read a GPIO input pin in a software polling loop and control GPIO output pins based on the input state. A button input is continuously monitored, and different LEDs are illuminated based on the button state.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| GPIOC | PC17 | Standard Input with internal pull-up (Button) |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC17 | GPIOC | USER_SWITCH_1 | N/A | <ul><li>PC17 is connected to Button S2<br><ul><li>Button S2 connects the pin to GND when pressed</ul></ul> |
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
1. Initializes PC27 and PC26 as digital outputs connected to the green and red LEDs
2. Initializes PC17 as a digital input with internal pull-up connected to Button S2
3. In an infinite loop:
   - Reads the state of the button input
   - If the button is pressed (input reads low), turns on the green LED
   - If the button is not pressed (input reads high), turns on the red LED

Compile, load and run the example. Press Button S2 to see the LED change from red to green. Release the button to see the LED change back to red.
