## Example Summary

Configures the IWDT module within the LFSS IP with a 500 ms period to be periodically reset by a
repeating timer G 4_0 interrupt (100 ms).

This IWDT has its own system independent power and clock source. If the application software does not successfully reset the watchdog within
the programmed time, the watchdog generates a POR reset to the SoC.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| IWDT |  |  |
| TIMER |  |  |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
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

The IWDT is configured with the following settings:
- Watchdog Source Clock = (LFOSC Freq) / (IWDT Clock Divider) = 32768Hz / 4 = 8.19 kHz
- Watchdog Period = (IWDT Clock Divider) ∗ (IWDT Period Count) / 32768Hz = 4 * 2^12 / 32768Hz = 500.00 ms

The TimerG 4_0 is configured with the following settings:
- Timer Clock Frequency = BUSCLK / (timerClkDivRatio * (timerClkPrescale + 1)) = 32000000Hz / (1 * (255 + 1)) = 125000 Hz
- Timer Period = 100 ms

Compile, load and run the example.

The green LED (PASS_LED) will toggle every time the IWDT is reset successfully (every 100ms). The example will wait for interrupts and does not abort.
