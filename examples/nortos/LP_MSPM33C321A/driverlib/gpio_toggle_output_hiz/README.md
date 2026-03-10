## Example Summary

This example demonstrates how to toggle a GPIO pin configured with Hi-Z (high impedance) output. A logic low is forced actively, but a logic high sets the pin as Hi-Z. This feature is useful for open-drain digital input/output applications. The example also toggles a standard output LED for comparison.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA2 | Standard Output (LED2 Blue) |
| GPIOA | PA15 | Hi-Z Output |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA2 | GPIOA | USER_LED_1 | N/A | <ul><li>PA2 is connected to LED2 Blue<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA15 | GPIOA | USER_TEST | N/A | <ul><li>PA15 is configured as Hi-Z output<br><ul><li>This pin requires an external pull-up resistor for proper operation</ul></ul> |
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
1. Initializes PA2 as a standard digital output connected to the blue LED
2. Initializes PA15 as a digital output with Hi-Z enabled
3. In an infinite loop:
   - Waits for approximately 0.5 seconds
   - Toggles both pins

When the Hi-Z pin is toggled:
- When set to logic low (0), the pin actively drives low
- When set to logic high (1), the pin enters high-impedance state and relies on an external pull-up resistor to reach logic high

Connect an external pull-up resistor to PA15. Compile, load and run the example. Observe the output of PA15 with an oscilloscope or logic analyzer. Note that without the pull-up resistor, the pin will remain in Hi-Z state when high, and the voltage level will be undefined.
