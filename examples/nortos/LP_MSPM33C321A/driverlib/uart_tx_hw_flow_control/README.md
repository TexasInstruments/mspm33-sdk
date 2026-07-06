## Example Summary

This example can be used with the uart_rx_hw_flow_control example running on another device.
This example sets up the UART with hardware flow control.

The application will begin by sending the first packet of data.
Since hardware flow control is enabled, the transmitter will automatically check that the CTS signal isasserted (low) before transmitting the next byte.
If CTS is de-asserted (high), the byte will not be transmitted out of the TX FIFO. Therefore we block if the TX FIFO is full before attempting to fill it.

After the first packet has been transmitted, the application will delay for some time to allow the UART Receiver application to re-configure its flow control settings.
After the delay, the application will begin to send the second packet of data.


## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL |  |  |
| UC5 | PA11 | RX Pin |
| UC5 | PA10 | TX Pin |
| UC5 | PA29 | RTS Pin |
| UC5 | PA30 | CTS Pin |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | PC26 | J8_72 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC27 | GPIOC | PC27 | J8_71 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
| PA11 | UC5 | RX | J4_33 | <ul><li>PA11 can be used as UART RX connected to XDS-110 backchannel or to boosterpack connector:<br><ul><li>To use UART on boosterpack connector we can use J4_33</ul></ul> |
| PA10 | UC5 | TX | J4_34 | <ul><li>PA10 can be used as UART TX connected to XDS-110 backchannel or to boosterpack connector:<br><ul><li>To use UART on boosterpack connector we can use J4_34</ul></ul>|
| PA29 | UC5 | RTS | J6_58 | N/A |
| PA30 | UC5 | CTS | J6_59 | N/A |
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
Connect this transmitter device to a receiver as follows:
- UART Transmitter TX  -> UART Receiver RX
- UART Transmitter CTS <- UART Receiver RTS

Compile, load and run the example.
Optionally, connect a logic analyzer or oscilloscope to observe hardware flow control.
Ensure that the UART Receiver example is running before starting this transmitter example.
Once the example is started, the UART Transmitter will start to transmit data.
