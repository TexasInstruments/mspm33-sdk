## Example Summary

The following example showcases two scenarios to use the oversampling feature. Three sequencers are configured to run a single SOC each. The first sequencer is not configured to use the oversampling feature. The second sequencer is configured to oversample 8 times, thus resulting in a 15-bit result, to improve accuracy. The third sequencer is configured to oversample 8 times and then right shift the result by 3. This results in a 12-bit result whose error is minimised in case of irregular voltage input to the ADC.

The results of all three scenarios in terms of voltages read will be printed on the uart console.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| HSADC0 | PA27 | HSADC_0 Channel 0 Pin |
| VREF | PA23 | VREF Positive (VREF+) |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA27 | HSADC0 | A0 | J27_7/P1 | <ul><li>PA27 can be connected to an active voltage source</ul> |
| PA23 | VREF | VREF+ | N/A | <ul><li>PA23 is used as the external voltage reference input for VREF (VREF+)<br><li>Connect a stable voltage in the range of 0V to 3.6V.</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
VREF is configured in external mode by default. Connect a stable external voltage reference (default 2V) to PA23 (VREF+) before running the example.
Compile, load and run the example. The debugger will stop at a SW breakpoint, and the user can verify the result.

On powerup, the ADC pin(s) used in this example are by default set to
the correct analog mode. Therefore, calls to
DL_GPIO_initPeripheralAnalogFunction for pinmuxing those pins are not needed.

Sample Output :
Sequencer 1 (without oversampling - 12 bit result) : 4091, 3296 mV
Sequencer 2 (with oversampling - 15 bit result) : 4091, 3296 mV
Sequencer 3 (with oversampling and right shift by 3 - 12 bit result) : 4092, 3296 mV

