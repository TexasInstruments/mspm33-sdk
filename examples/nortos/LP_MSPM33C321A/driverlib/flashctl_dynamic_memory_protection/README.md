## Example Summary

This example demonstrates dynamic protection against program and erase operations.

Dynamic write protection provides a way to specify which sectors should be protected from modification against any program or erase operations which are not purposefully issued by the flash controller. This example protects a sector, attempts to erase and then program that sector, and shows that these operations fail because the sector was dynamically protected.

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
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage

Compile, load and run the example.
The flash operations will start automatically when the example is run.
The application will hit a breakpoint if there is a failure.
LED2 Green (PASS_LED) will turn on after all flash operations are completed successfully.
LED2 Red (FAIL_LED) will turn on if any flash operation fails.

## Application Design Details

This example demonstrates the following FlashCTL operations related to dynamic memory protection:

1. Protecting main memory using DL_FlashCTL_protectMainMemory
2. Attempting to erase protected memory (expected to fail)
3. Protecting a specific sector using DL_FlashCTL_protectSector
4. Attempting to program protected memory (expected to fail)
5. Unprotecting a sector using DL_FlashCTL_unprotectSector
6. Erasing the unprotected sector (expected to succeed)
7. Unprotecting the sector again (since Flash Controller automatically re-protects memory after operations)
8. Programming data to the unprotected sector (expected to succeed)

The example uses the following FlashCTL APIs from dl_flashctl.h:
- DL_FlashCTL_protectMainMemory: Protects all main memory from erase/program operations
- DL_FlashCTL_protectSector: Protects a specific sector from erase/program operations
- DL_FlashCTL_unprotectSector: Unprotects a specific sector to allow erase/program operations
- DL_FlashCTL_eraseMemoryFromRAM: Erases a sector of memory
- DL_FlashCTL_programMemoryFromRAM32: Programs 32-bit data to flash
- DL_FlashCTL_getFailureStatus: Gets the reason for a command failure

The example demonstrates proper error handling for flash operations, checking the status and failure type after each operation and setting appropriate error flags:
- If all operations succeed as expected (including the expected failures), the PASS_LED (LED2 Green) is turned on
- If any operation doesn't behave as expected, the FAIL_LED (LED2 Red) is turned on and a breakpoint is triggered

This example highlights the importance of properly managing flash memory protection to ensure data integrity and security in embedded applications.
