## Example Summary

This example configures TimerG (TIMG8_0) in periodic mode and toggles the LED2
every 500ms. The device remains in SLEEP mode while waiting
for an interrupt, demonstrating low-power operation with timer functionality.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (LED2 Green) |
| GPIOC | PC26 | Standard Output (LED2 Red) |
| SYSCTL |  |  |
| TIMG8_0 |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | LED2 Green | LED2 Green | <ul><li>LED2 Green will toggle every 500ms</ul> |
| PC26 | GPIOC | LED2 Red | LED2 Red | <ul><li>LED2 Red will toggle every 500ms</ul> |
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
Compile, load and run the example.
LED2 (Green and Red) will toggle every 500ms, and the device will remain in
SLEEP mode while waiting for an interrupt.

The example configures TIMG8_0 with the following settings:
- Timer mode: Periodic
- Timer period: 500 ms
- Clock source: BUSCLK (32 MHz)
- Clock prescale: 256
- Resulting timer clock: 125 kHz (32 MHz / 256)
- Timer load value: 62499 (calculated as (500 ms * 125 kHz) - 1)
- Interrupt enabled on timer zero event

When the timer reaches zero, it triggers an interrupt that toggles the LEDs and then
reloads the timer value automatically (periodic mode), demonstrating the use of timer
interrupts in a low-power application.
