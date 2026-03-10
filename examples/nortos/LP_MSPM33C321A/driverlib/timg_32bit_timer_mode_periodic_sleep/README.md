## Example Summary

This example configures TimerG12_0 in 32-bit periodic mode and toggles
the LED every 500ms. The device remains in SLEEP mode while waiting for an interrupt,
demonstrating efficient power management with timer-based wake-up.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (LED2 Green) |
| GPIOC | PC26 | Standard Output (LED2 Red) |
| SYSCTL |  |  |
| TIMG12_0 |  | 32-bit Timer |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | LED2 Green | <ul><li>LED2 Green will toggle every 500ms</ul> |
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

This example demonstrates:
1. 32-bit timer configuration in periodic mode
2. Power-efficient operation using SLEEP mode
3. Timer-based wake-up for periodic tasks

The timer is configured with:
- Timer instance: TIMG12_0
- Timer load value: 15,999,999 (for 500ms period with 32MHz clock)
- Interrupt on ZERO event

LED2 Green will toggle every 500ms as the timer generates interrupts.
Between interrupts, the device remains in SLEEP mode to conserve power.
