## Example Summary

This demo shows how to program a MSPM33C321A MCU using a LP-MSPM33C321A as host by BSL UART interface.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOB | PB21 | Standard Input with internal pull-up (Button) |
| GPIOB | PB0 | Standard Output (BSL NRST) |
| GPIOB | PB16 | Standard Output (BSL Invoke) |
| GPIOA | PA28 | Open-Drain Output (LED PA0) |
| GPIOB | PB27 | Standard Output (LED Error) |
| GPIOA | PA5 | Standard Output (LED) |
| SYSCTL |  |  |
| UART (UC1_0) | PA11 | RX Pin |
| UART (UC1_0) | PA10 | TX Pin |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PB21 | GPIOB | PB21 (Button) | J4_33 | <ul><li>PB21 is connected to S2 button to GND with no external pull resistor<br><ul><li>Press `S2` button to connect pin to GND<br><li>Don't use `S2` button if not needed by application</ul></ul> |
| PB0 | GPIOB | PB0 (BSL NRST) | J1_9 | N/A |
| PB16 | GPIOB | PB16 (BSL Invoke) | J1_5 | N/A |
| PA28 | GPIOA | PA28 (LED PA0) | J4_40 | <ul><li>PA28 is 5V tolerant open-drain so it requires pull-up<br><ul><li>`J10 2:3` Use 3.3V pull-up<br><li>`J10 1:2` Use 5V pull-up</ul></ul> |
| PB27 | GPIOB | PB27 (LED Error) | J1_2 | <ul><li>PB27 can be connected to LED2 Blue<br><ul><li>`J13 ON` Connect to LED2 Blue<br><li>`J13 OFF` Disconnect from LED2 Blue</ul></ul> |
| PA5 | GPIOA | PA5 (LED) | J2_12 | N/A |
| PA11 | UART (UC1_0) | RX | J4_35 | N/A |
| PA10 | UART (UC1_0) | TX | J4_36 | N/A |
| PA20 | DEBUGSS | SWCLK | J2_13 | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | J2_17 | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

### Device Migration Recommendations
This project was developed for the MSPM33C321A device included in the LP_MSPM33C321A LaunchPad.


### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.


## Example Usage

This example requires the usage of two LaunchPads. One of them will run the host application, which will invoke BSL on the other board. The host application will communicate with the target device using various BSL commands.

PA10 and PA11 are UART TX and RX pins on both the host device and target device. Connect the Host's TX pin PA10 to the target's RX pin PA11. Then connect the Host's RX pin PA11 to the target's TX pin PA10.

Compile and load the example on the BSL host device. When the program has halted at main(), perform the below steps:
1. On the LP-MSPM33C321A, hold the BSL Invoke button(S1), and then trigger a system reset by pressing the NRST button(ensuring that the BSL Invoke button is held while pressing the NRST button). Then let go of both buttons
2. Within 4 seconds of completing Step 1, continue execution of the program on the host LP.

If Step 1 and Step 2 are performed successfully, users will be able to connect to the target device and observe that its Flash has been programmed.

> NOTE : Ensure that your target device does not have its Flash empty, one may load any dummy application(which does not enforce any GSC or TrustZone protections on flash) onto the flash of the target device.
