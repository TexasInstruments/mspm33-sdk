## Example Summary

This example demonstrates UART TX with RS485 mode. RS485 is usable with both UART Main and UART Extend.

This example can be used with the uart_rs485_receive_packet example running on another device.

This example sets driver enable (DE) pin high, then transmits a packet of data. The device then sets the DE pin back to low.

When all bytes have been transmitted, LED1 will toggle.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOA | PA0 | Open-Drain Output |
| GPIOB | PB4 | Standard Output |
| SYSCTL |  |  |
| UC12 | PA11 | RX Pin |
| UC12 | PA10 | TX Pin |
| UC12 | PA8 | RTS Pin |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA0 | GPIOA | PA0 | J27_9 | <ul><li>PA0 is 5V tolerant open-drain so it requires pull-up<br><ul><li>`J19 1:2` Use 3.3V pull-up<br><li>`J19 2:3` Use 5V pull-up</ul><br><li>PA0 can be connected to LED1<br><ul><li>`J4 ON` Connect to LED1<br><li>`J4 OFF` Disconnect from LED1</ul></ul> |
| PB4 | GPIOB | PB4 | J4_40 | N/A |
| PA11 | UC12 | RX | J4_33 | <ul><li>PA11 can be used as UART RX connected to XDS-110 backchannel or to boosterpack connector:<br><ul><li>To use UART on boosterpack connector we can use J4_33</ul></ul> |
| PA10 | UC12 | TX | J4_34 | <ul><li>PA10 can be used as UART TX connected to XDS-110 backchannel or to boosterpack connector:<br><ul><li>To use UART on boosterpack connector we can use J4_34</ul></ul> |
| PA8 | UC12 | RTS | J1_4 | N/A |
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
Connect the device transmitting the RS485 packet to the receiving device using the following setup guidelines. You will need two RS485 transceiver boards, one for each MCU.
- Transmitting MCU 3.3V         -> RS485 transceiver #1 3.3V
- Transmitting MCU GND          -> RS485 transceiver #1 GND
- Transmitting MCU TX (PA10)    -> RS485 transceiver #1 RX
- Transmitting MCU DE (PB4)     -> RS485 transceiver #1 R/T
- Receiving MCU 3.3V            -> RS485 transceiver #2 3.3V
- Receiving MCU GND             -> RS485 transceiver #2 GND
- Receiving MCU RX (PA11)       -> RS485 transceiver #2 TX
- Receiving MCU RE (PB3)        -> RS485 transceiver #2 R/T
- (+) terminal of RS485 transceiver #1 -> (+) terminal of RS485 transceiver #2
- (-) terminal of RS485 transceiver #1 -> (-) terminal of RS485 transceiver #2

Compile, load and run the example. This example can be used in conjunction with the uart_rs485_receive_packet example running on a separate device. Ensure that the uart_rs485_receive_packet is running first on the other device.
