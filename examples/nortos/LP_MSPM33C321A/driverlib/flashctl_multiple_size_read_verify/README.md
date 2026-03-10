## Example Summary

This example demonstrates the read verify command for different data sizes, and with and without ECC.

The read verify command may be used to read a flash location and compare the data to data which is preloaded into the CMDDATA registers of the flash controller. This example shows read verify operations on multiple data sizes (8/16/32/64/96/128-bit) and demonstrates both successful read verify operations and expected failures.

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

## Application Design Details

This example demonstrates the following FlashCTL operations related to read verify:

1. Programming and Read Verify of Different Data Sizes:
   - 8-bit data using DL_FlashCTL_programMemoryFromRAM8 and DL_FlashCTL_readVerifyFromRAM8
   - 16-bit data using DL_FlashCTL_programMemoryFromRAM16 and DL_FlashCTL_readVerifyFromRAM16
   - 32-bit data using DL_FlashCTL_programMemoryFromRAM32 and DL_FlashCTL_readVerifyFromRAM32
   - 64-bit data using DL_FlashCTL_programMemoryFromRAM64 and DL_FlashCTL_readVerifyFromRAM64
   - 96-bit data using DL_FlashCTL_programMemoryFromRAM96 and DL_FlashCTL_readVerifyFromRAM96
   - 128-bit data using DL_FlashCTL_programMemoryFromRAM128 and DL_FlashCTL_readVerifyFromRAM128

2. Demonstrating Read Verify Failure:
   - Attempting to verify 128-bit data with incorrect data (expected to fail)
   - Checking that the failure type is DL_FLASHCTL_FAIL_TYPE_VERIFY_ERROR

The example uses the following FlashCTL APIs from dl_flashctl.h:
- DL_FlashCTL_unprotectSector: Unprotects a sector for erase/program operations
- DL_FlashCTL_eraseMemoryFromRAM: Erases a sector of memory
- DL_FlashCTL_programMemoryFromRAM8/16/32/64/96/128: Programs data of various sizes to flash
- DL_FlashCTL_readVerifyFromRAM8/16/32/64/96/128: Verifies data of various sizes in flash
- DL_FlashCTL_getFailureStatus: Gets the reason for a command failure

The example demonstrates proper error handling for flash operations, checking the status after each operation and setting appropriate error flags:
- If all operations succeed (including the expected failure), the PASS_LED (LED2 Green) is turned on
- If any operation doesn't behave as expected, the FAIL_LED (LED2 Red) is turned on and a breakpoint is triggered

This example highlights the read verify capability of the flash controller, which is useful for verifying data integrity after programming operations or for checking if data in flash matches expected values.
