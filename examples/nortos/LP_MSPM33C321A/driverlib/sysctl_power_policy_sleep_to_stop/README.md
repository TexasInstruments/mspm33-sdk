## Example Summary

The following example explains how to change the power policy from SLEEP
to STOP during runtime. There are two timers in use: TIMG4_0 and TIMG4_1.
TIMG4_1 is sourced from the BUSCLK(ULPCLK), it runs faster in RUNSLEEP and slower in STOP mode.
TIMG4_0 is sourced from the MFCLK, and therefore can run in same speed in RUNSLEEP and STOP mode.
The application will start in SLEEP, and then every 5s the power policy will
switch to STOP. The power policies will continue to alternate every 5s.
TIMG4_0 will toggle the LED1(GREEN LED) constantly with the same speed during both SLEEP and
STOP.
As TIMG4_1 frequency changes in both modes, the LED2 or RED LED will toggle faster in RUNSLEEP and slower in STOP mode.
CLK_OUT source is configured to be ULPCLK. If MCLKDIVCFG[26:24] is not configured, then in SLEEP. ULPCLK will give 32 MHZ and in STOP, it will give 4 MHZ. CLKOUT has a dividor of 2. In oscilloscope, one will be able to see, (32/2)MHZ = 16MHZ in SLEEP and (4/2)MHZ = 2 MHZ in STOP, periodically.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL | PA9 | CLK OUT |
| SYSCTL |  |  |
| TIMG4_0 |  |  |
| TIMG4_1 |  |  |
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
Compile, load the example. Then disconnect the debugger for the core to go to LPM.
Monitor the behavior of LED1 and LED2.
LED2(RED) will toggle faster in RUNSLEEP and slower in STOP mode.
LED1(GREEN) will continue to toggle with the same speed.
