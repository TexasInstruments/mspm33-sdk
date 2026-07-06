## Example Summary

This example configures TimerG (TIMG4_0) in one-shot mode and
turns on the PASS LED (Green) after 500 ms. The device remains in STANDBY mode while waiting
for an interrupt, demonstrating low-power operation with timer functionality.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (LED Green) |
| GPIOC | PC26 | Standard Output (LED Red) |
| SYSCTL |  |  |
| TIMG4_0 |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS LED (Green) | LED Green | <ul><li>PASS LED (Green) will turn on after 500 ms</ul> |
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
Compile, load and run the example. The PASS LED (Green) starts off and turns on after 500 ms,
then the device will remain in STANDBY mode.

The example configures TIMG4_0 with the following settings:
- Timer mode: One-shot
- Timer period: 500 ms
- Clock source: LFCLK (32.768 kHz)
- Clock prescale: 255 (prescale register value, actual divisor is 256)
- Resulting timer clock: 128 Hz (32768 Hz / 256)
- Timer load value: 63 (calculated as (500 ms * 128 Hz) - 1)
- Interrupt enabled on timer zero event

When the timer reaches zero, it triggers an interrupt that turns on the PASS LED (Green),
demonstrating the use of timer interrupts in a low-power application.
