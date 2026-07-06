## Example Summary

This example is used to flash CSC bootloader. The Customer Secure Code (CSC) is an implementation of the publicly available mcuboot for devices in the MSP Family with advanced security features, such as the device family MSPM33, to allow for customers to have additional and more advanced secure booting features in their development and deployment. When paired with an application image such as this example, this represents a full solution for updates and verification of new images on the device. However, the customer secure code rigorous definition only refers to the verification of the image and configuring security settings, not loading the image onto the device. The full set of features and execution flow will vary depending on the specific device used and features present on the device.

Note that this example needs to be used along with another CSC application example customer_secure_sample_image. Both need to be built and flashed independently, the application image needs to be flashed at a particular location as specified in examples/boot_manager/customer_secure/customer_secure_code/flash_map_backend/flash_map_backend.c for this example to work successfully.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (PASS_LED) |
| GPIOC | PC26 | Standard Output (FAIL_LED) |
| SYSCTL |  |  |
| FLASHCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PC27 (PASS_LED) | LED2 Green | <ul><li>LED2 Green is used to indicate successful operation</ul> |
| PC26 | GPIOC | PC26 (FAIL_LED) | LED2 Red | <ul><li>LED2 Red is used to indicate failed operation</ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Device Migration Recommendations
This project was developed for the MSPM33C321A device included in the LP_MSPM33C321A LaunchPad.

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau869).

## Example Usage

Compile, load and run the example. The boot operations will start automatically when the device is turned on or on a reset. The application will hit a breakpoint if there is a failure. LED2 Green (PASS_LED) will turn on after all boot operations are completed successfully. As CSC is authenticating and jumping to a sample application image, that example has LED Blue turned on. So as soon as the application control is switched to sample application, LED Blue can be seen turned on. Refer CSC User Guide for further details. 

## Application Design Details

- Verify the application image with either MLDSA-87, ECDSA-256, or both.
- The corresponding public keys that are used to sign the application image should be modified in boot_keys.c in this example.
- Sample pvt keys that are used to sign the application and its corresponding public keys is already present in the example.
- For further details refer CSC User Guide. 
