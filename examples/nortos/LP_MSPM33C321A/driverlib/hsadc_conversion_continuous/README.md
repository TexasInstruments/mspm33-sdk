## Example Summary

The example triggers conversion on HSADC0 instance continuously using software trigger. The interrupt from a conversion is followed by another conversion trigger after storing the result result.

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

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.Please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA27 | HSADC0 | A0 | J27_7/P1 | <ul><li>PA27 can be connected to an active voltage source</ul> |
| PA23 | VREF | VREF+ | N/A | <ul><li>PA23 is used as the external voltage reference input for VREF (VREF+)<br><li>Connect a stable voltage in the range of 0V to 3.6V.</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for the MSPM33C321A device included in the LP_MSPM33C321A LaunchPad.


please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
VREF is configured in external mode by default. Connect a stable external voltage reference (default 2V) to PA23 (VREF+) before running the example. 
Compile, load and run the example. Connect vaild inputs ranging from 0-VCC to the ADC channels. The program runs indefinitely and the input value is reflected in the gADCResult variable.

On powerup, the ADC pin(s) used in this example are by default set to
the correct analog mode. Therefore, calls to
DL_GPIO_initPeripheralAnalogFunction for pinmuxing those pins are not needed.
