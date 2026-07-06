## Example Summary
This example demonstrates how to receive CAN Classic and CAN-FD
messages using standard IDs.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL | PA5 | HFXIN |
| SYSCTL | PA6 | HFXOUT |
| EVENT |  |  |
| CANFD1 | PC22 | RX Pin |
| CANFD1 | PC21 | TX Pin |
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
| PC22 | CANFD1 | CANRX | J15_31 |  |
| PC21 | CANFD1| CANTX | J15_32 |  |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for a superset device included in the LP_MSPM33C321A LaunchPad. Please
visit the [CCS User's Guide](https://software-dl.ti.com/msp430/esd/MSPM33-SDK/latest/docs/english/tools/ccs_ide_guide/doc_guide/doc_guide-srcs/ccs_ide_guide.html#sysconfig-project-migration)
for information about migrating to other MSPM33 devices.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage
Connect the CAN_RX and CAN_TX to TXD and RXD of TCAN332DEVM, and to the canh and canl of the EVM, connect to PCAN and the PCAN will be connected to the laptop's one port.
According to TCAN332D EVM's user guide[https://www.ti.com/lit/ug/sllu234a/sllu234a.pdf], as the MCAN has 120 ohm power resistor, short JMP4 and open JMP5. 

The example has the following Nominal and Data Rate frequencies configured:
- Nominal Bit Rate (250 KBits/s)
- Data Bit Rate    (2 MBits/s)

Compile, load and run the example.

To turn on the LED_1, the example must receive the following package:
- ID : 0x03
- DATA[0] = 0x01

To turn off the LED_1, the example must receive the following package:
- ID : 0x03
- DATA[0] = 0x00

To turn on the LED_2, the example must receive the following package:
- ID : 0x04
- DATA[0] = 0x01

To turn off the LED_2, the example must receive the following package:
- ID : 0x04
- DATA[0] = 0x00

Note: There is a known limitation where the example might stop receiving
packages if the application halt execution after hitting a breakpoint. It is
highly recommended to use free run option while testing the example.
