## Example Summary

This example demonstrates PIR sensor data capture using the Device Agent Protocol
(DAP) for communication with Edge AI Studio. The application initializes the DAP
stack with UART HAL in interrupt mode and streams ADC sensor data when streaming
is enabled by the host.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| SYSCTL |  |  |
| UART0 | PA10 | TX Pin (Back-channel) |
| UART0 | PA11 | RX Pin (Back-channel) |
| GPIO | PC26 | LED RED |
| GPIO | PA2 | LED BLUE |
| GPIO | PC27 | LED GREEN |
| HSADC | ADC0 CH10 | PIR Sensor Input |
| TIMER | TIMA0_0 | ADC Sampling Timer |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA10 | UART0 | TX | J8_80 | <ul><li>PA10 can be used as UART TX for back-channel communication</ul> |
| PA11 | UART0 | RX | J8_79 | <ul><li>PA11 can be used as UART RX for back-channel communication</ul> |
| PC26 | GPIO | LED RED | J8_72 | <ul><li>This pin can be used for testing purposes</ul> |
| PA2 | GPIO | LED BLUE | J8_75 | <ul><li>This pin can be used for testing purposes</ul> |
| PC27 | GPIO | LED GREEN | J8_71 | <ul><li>This pin can be used for testing purposes</ul> |
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

1. Connect the LP_MSPM33C321A LaunchPad to your PC via USB.
2. Build and flash the example using CCS or the SDK build system.
3. Open Edge AI Studio and connect to the device via the back-channel UART.
4. Start streaming to capture PIR sensor data.

## References

- MSPM33C321A Technical Reference Manual [Link](https://www.ti.com/product/MSPM33C321A)
- [Edge AI Studio](https://dev.ti.com/edgeaistudio/)

