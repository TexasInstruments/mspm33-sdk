## Example Summary

This example configures TimerA0_0 in periodic mode and
toggles GPIO every 2s using the repeat count feature.
NOTE: The repeat count feature is specific to the TimerA0 instance, and not the
other TimerA instances.
The device remains in SLEEP mode while waiting for an interrupt.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (LED2 Green) |
| GPIOC | PC26 | Standard Output (LED2 Red) |
| SYSCTL |  |  |
| TIMA0_0 |  | Timer with Repeat Count |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | LED2 Green | <ul><li>LED2 Green will toggle every 2s</ul> |
| PC26 | GPIOC | FAIL_LED | LED2 Red | <ul><li>Not used in this example</ul> |
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

This example demonstrates the TimerA0_0 repeat count feature:
- Timer configured in periodic mode with 500ms period
- Clock divider of 8 and prescaler of 256
- Repeat counter set to 4 (triggers interrupt after 4 periods = 2s)

LED2 (Green) will toggle every 2 seconds as the repeat counter triggers an interrupt.
