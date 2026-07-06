## Example Summary
The following example configures TimerG in QEI mode (2-pin mode). The device remains in SLEEP0 mode after
QEI is configured, and will turn an LED on or off depending on motor direction.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Output |
| SYSCTL |  |  |
| TIMG8_0 | PB21 | Capture/Compare Pin 0 |
| TIMG8_0 | PA2 | Capture/Compare Pin 1 |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | Output | LED2 (Green) | N/A |
| PB21 | TIMG8_0 | CCP0 | J1_39 | N/A |
| PA2 | TIMG8_0 | CCP1 | J1_7 | N/A |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide].

## Example Usage
Compile, load and run the example. Attach quadrature encoder. Observe on board
LED changing when motor direction goes from clockwise to counter clockwise or
vice versa
