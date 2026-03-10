## Example Summary
Demonstrate more advanced LittlevGL graphics capabilities by creating a demo User Interface complete with touchscreen interactions.
This demo uses TFT display, which utilizes the [ST7796 display driver](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf) and [DFT6636U capacitive touch driver](https://cdn-shop.adafruit.com/product-files/5826/FT6336U-DataSheet-V1.1.pdf) .

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (PASS_LED) |
| GPIOC | PC26 | Standard Output (FAIL_LED) |
| SYSCTL |  |  |
| SPI |  |  |
| I2C |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.
Refer the  [Application note](https://www.ti.com/lit/SDAA158) to get the details of hardware connections.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J101 15:16 ON` Connect to XDS-110 SWCLK while debugging<br><li>`J101 15:16 OFF` Disconnect from XDS-110 SWCLK if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J101 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J101 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

## LVGL Example Hardware Connections
| LP-MSPM33C321A | TFT Display |
| --- | --- |
| PA8 | SCK |
| PA9 | SDI |
| PA10 | LCD_RST |
| PB3 | Not Used |
| PB6 | LCD SCS |
| PB30 | LCD RS |
| PC5(Not Used) | INT |
| PA0 | CTP_SDA |
| PA1 | CTP_SDL |
| PC4 | CTP_RST |

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
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/slau962).

## Example Usage
* Run the example.

* The demo GUI will be displayed with various touchscreen widgets. There are 3 different tab views that can be switched to via touch.

## Rebuilding the LVGL library
* LVGL library can be rebuilt for different configuration than used by TI. In order to do it, follow below steps:

* Make necessary changes in lv_conf.h file and recompile the project. LVGL source code is already part of example.
