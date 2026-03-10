## Example Summary

COMP is configured to trigger a PWM fault when positive input exceeds AVCC/2.
The 8-bit DAC is used to set the negative threshold of the comparator while the
positive input is connected to an external pin.
A PWM will be driven when the comparator input is below AVCC/2 and then stop
when the input goes above this threshold.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| TIMA0 | PA8 | Capture/Compare Pin 0 |
| VREF | PA23 | VREF Positive (VREF+) |
| COMP0 | PA26 | COMP Positive Channel 0 Pin |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.
For MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA8 | TIMA0 | CCP0 | J2_15 | N/A |
| PA23 | VREF | VREF+ | N/A | <ul><li>PA23 can be connected to GND using C7 or to VREF- using C8<br><ul><li>`C8` is soldered by default connecting pin to VREF-<br><li>`C7` is not soldered by default and can be used for a custom capacitor to GND</ul><br><li>PA23 can be connected to boosterpack connector<br><ul><li>`R22` is not soldered by default<br><li>Solder `R22` to connect pin to J28_10</ul></ul> |
| PA26 | COMP0 | IN0+ | J1_5 | <ul><li>PA26 can be connected to photodiode D1<br><ul><li>`J18 OFF` Disconnect from photodiode D1</ul><br><li>PA26 can be connected to thermistor RT1<br><ul><li>`J9 1:2/OFF` Disconnect from thermistor RT1</ul></ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad.

## Example Usage
Connect COMP0_IN0+ to an external signal.
Compile, load and run the example.
Check PWM signals using an oscilloscope.
PWM will stop when the comparator input exceeds AVCC/2.

On powerup, the COMP pin(s) used in this example are by default set to
the correct analog mode. Therefore, calls to
DL_GPIO_initPeripheralAnalogFunction for pinmuxing those pins are not needed.
