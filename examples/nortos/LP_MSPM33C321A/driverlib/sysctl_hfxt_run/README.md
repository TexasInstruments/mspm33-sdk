## Example Summary

HFXT (40MHz in LaunchPad) is a source for HFCLK, which sources MCLK.
HFCLK/4 is output to CLK_OUT pin that can be measured.
HFXT -> HFCLK (4-48MHz)
CLK_OUT divider is 4. so CLK_OUT is 40/4 = 10 MHZ.

TimerG is configured in periodic mode and toggles an LED every 500 ms.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL | PA9 | CLK OUT |
| SYSCTL | PA5 | HFXIN |
| SYSCTL | PA6 | HFXOUT |
| TIMG4_0 |  |  |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | PC26 | J8_72 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC27 | GPIOC | PC27 | J8_71 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
| PA9 | SYSCTL | CLK_OUT | J8_80 | N/A |
| PA5 | SYSCTL | HFXIN | J11_9 | <ul><li>PA5 is used for HFXT functionality by default in Launchpad<br><ul><li>`Y2` crystal is soldered and connected by default</ul><br><li>PA5 can be connected to J11_9<br><ul><li>`R15` is not soldered soldered by default<br><li>Solder `R15` to connect pin to J11_9</ul> |
| PA6 | SYSCTL | HFXOUT | J11_10 | <ul><li>PA6 is used for HFXT functionality by default in Launchpad<br><ul><li>`Y2` crystal is soldered and connected by default</ul><br><li>PA6 can be connected to J11_10<br><ul><li>`R18` is not soldered soldered by default<br><li>Solder `R18` to connect pin to J11_10</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/mspm33/esd/MSPM33-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM33 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Compile, load and run the example. LED toggles every 500 ms.
Measure the digital output of CLK_OUT.
