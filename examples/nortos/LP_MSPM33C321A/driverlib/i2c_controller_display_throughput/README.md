## Example Summary

I2C Controller transmits data to a target device and measures the I2C bus throughput.
The example uses DMA for data transfer.
This example is intended to be used with the i2c_target_display_throughput example running on another device.
The Target device must be enabled and active before running this example.
## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL |  |  |
| UC15_0 | PA0 | I2C Serial Data line (SDA) |
| UC15_0 | PA1 | I2C Serial Clock line (SCL) |
| UC12 | PA10 | UART TX |
| UC12 | PA11 | UART RX |
| DMA0 |  |  |
| EVENT |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | PC26 | J8_72 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC27 | GPIOC | PC27 | J8_71 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
| PA0 | UC15_0 | SDA | J5_50 | N/A |
| PA1 | UC15_0 | SCL | J5_49 | N/A |
| PA10 | UC12 | TX | J4_34 | <ul><li>PA10 can be used as UART TX connected to XDS-110 backchannel or directly to UART connector<br><ul><li>`J101 7:8 ON` Connect UART TX backchannel to XDS-110</ul></ul> |
| PA11 | UC12 | RX | J4_33 | <ul><li>PA11 can be used as UART RX connected to XDS-110 backchannel or directly to UART connector<br><ul><li>`J101 9:10 ON` Connect UART RX backchannel to XDS-110</ul></ul> |
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

Connect SDA and SCL between I2C Controller and Target:
- Controller SDA -> Target SDA
- Controller SCL -> Target SCL

Compile, load and run the example.

Ensure that the i2c_target_display_throughput example is running **before** starting the I2C Controller example.

Open a serial terminal on the Controller device with the following settings:
- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1

The I2C throughput measurement results will be displayed on the serial terminal.
The expected output will show the throughput in MHz format:
```
Throughput: XX.XXXXXXXX MHz
```
