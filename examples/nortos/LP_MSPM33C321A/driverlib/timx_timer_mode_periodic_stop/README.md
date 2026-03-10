## Example Summary

This example configures TimerG (TIMG4_0) in periodic mode and
toggles the PASS LED (Green). The period starts at 500ms and decreases by 50ms each toggle
until the period is 100 ms, and then repeats. The device remains in STOP mode
while waiting for an interrupt, demonstrating low-power operation with timer functionality.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (PASS LED Green) |
| GPIOC | PC26 | Standard Output (FAIL LED Red) |
| SYSCTL |  |  |
| TIMG4_0 |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS LED (Green) | LED Green | <ul><li>PASS LED (Green) will toggle with decreasing period</ul> |
| PC26 | GPIOC | FAIL LED (Red) | LED Red | <ul><li>FAIL LED (Red) remains off throughout the example</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for the MSPM33C321A device included in the LP_MSPM33C321A LaunchPad.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Compile, load and run the example. The PASS LED (Green) will toggle increasingly rapidly.
The period starts at 500ms and decreases by 50ms each toggle until the period is 100 ms,
and then repeats. The device remains in STOP mode while waiting for an interrupt.

The example configures TIMG4_0 with the following settings:
- Timer mode: Periodic
- Timer period: 500 ms (initially)
- Clock source: MFCLK (4 MHz)
- Clock prescale: 255 (prescale register value, actual divisor is 256)
- Resulting timer clock: 15625 Hz (4000000 Hz / 256)
- Timer load value: 7812 (calculated as (500 ms * 15625 Hz) - 1)
- Interrupt enabled on timer zero event

When the timer reaches zero, it triggers an interrupt that toggles the PASS LED (Green) and then
adjusts the timer period for the next cycle. The timer automatically reloads the value
(periodic mode), demonstrating the use of timer interrupts in a low-power application.
