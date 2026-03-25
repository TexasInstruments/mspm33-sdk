## Example Summary

Note: The use of "Master" and "Slave", along with "MOSI/MISO" terminology is being considered obsolete. These terms will be replaced with "Controller" and "Peripheral", and "PICO/POCI" respectively.

The following example configures the SPI as a Controller.
This example is intended to be used with the spi_peripheral_display_throughput example running on another device.

This example sets up the SPI to trigger the DMA Channel 1 to do a transmit data transfer and the DMA Channel 0 to do a receive data transfer repeatedly.

The DMA Channel 1 is configured in single transfer mode, using the static SPI TX DMA event as the DMA trigger.
When the SPI TXINT is set (which indicates that the SPI is ready to transmit data), this triggers the DMA to transfer the data from a user buffer to the SPI TXFIFO. This transfer will start as soon as the channel is enabled, because the SPI TXINT should be set when the SPI is enabled.
The DMA Channel 0 is configured in single transfer mode, using the static SPI RX DMA event as the DMA trigger.
When data is received and the SPI RXINT is set, this triggers the DMA to transfer the data from the SPI RXFIFO to a user buffer.

After SPI_PACKET_SIZE bytes have been transferred by the DMA, this will trigger the DL_SPI_IIDX_DMA_DONE_TX interrupt indicating that the DMA is done transferring all expected bytes from the buffer to the TXFIFO.
The SPI Controller will then initiate the transaction. The SPI will transmit the data that is in the TXFIFO until it is empty. When the TXFIFO is empty, this will trigger the DL_SPI_IIDX_TX_EMPTY interrupt indicating that the transfer is complete.

When the SPI Controller receives SPI_PACKET_SIZE bytes from the SPI Peripheral, this will trigger the DL_SPI_IIDX_DMA_DONE_RX interrupt indicating that the DMA is done transferring all expected bytes from the RXFIFO to the buffer.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC26 | Standard Output |
| GPIOC | PC27 | Standard Output |
| SYSCTL |  |  |
| UC12 | PA10 | UART TX |
| UC12 | PA11 | UART RX |
| UC2 | PA8 | SPI SCLK (Clock) |
| UC2 | PA9 | SPI PICO (Peripheral In, Controller Out) |
| UC2 | PB5 | SPI POCI (Peripheral Out, Controller In) |
| UC2 | PB6 | SPI CS1 (Chip Select 1) |
| EVENT |  |  |
| DMA0 |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC26 | GPIOC | PC26 | J8_72 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul> |
| PC27 | GPIOC | PC27 | J8_71 | <ul><li>This pin can be used for testing purposes<ul><li>Pin can be reconfigured for general purpose as necessary</ul></ul>|
| PA10 | UC12 | TX | J4_34 | <ul><li>PA10 can be used as UART TX connected to XDS-110 backchannel or directly to UART connector<br><ul><li>`J101 7:8 ON` Connect UART TX backchannel to XDS-110</ul></ul> |
| PA11 | UC12 | RX | J4_33 | <ul><li>PA11 can be used as UART RX connected to XDS-110 backchannel or directly to UART connector<br><ul><li>`J101 9:10 ON` Connect UART RX backchannel to XDS-110</ul></ul> |
| PA8 | UC2 | SCLK |  | N/A |
| PA9 | UC2 | PICO |  | N/A |
| PB5 | UC2 | POCI |  | N/A |
| PB6 | UC2 | CS1 |  | N/A |
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
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide].

## Example Usage
Make the following connections between the SPI Controller and SPI Peripheral:
- Controller SCLK -> Peripheral SCLK
- Controller PICO -> Peripheral PICO
- Controller POCI <- Peripheral POCI
- Controller CS   -> Peripheral CS

Compile, load and run the example.

Ensure that the SPI Peripheral example is running **before** starting the SPI Controller example.

Open a serial terminal on the Controller device with the following settings:
- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1

The SPI throughput measurement results will be displayed on the serial terminal.
The expected output will show the throughput in Mbps format:
```
Throughput: XX.XXXXXXXX Mbps
```
