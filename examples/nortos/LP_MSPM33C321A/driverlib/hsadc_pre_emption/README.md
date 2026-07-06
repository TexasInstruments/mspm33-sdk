## Example Summary

The following example demonstrates the pre-emption of sequencers in ADC due to the inbuilt priority mechanism. We enable the preemption without restart mode in HSADC0, which allows the sequencer operation to be pre-empted without allowing the pre-empted sequencer to run again. 

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
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
Compile, load and run the example. Then we trigger sequencer 0 and sequencer 1 together and verify the conversions. The debugger will stop at a SW breakpoint, and the user can verify whether the pre-emption took place or not using the gCheckPreempt variable.
