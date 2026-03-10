## Example Summary

This example demonstrates programming multiple size data (8/16/32/64/96/128-bit) to flash with automatic hardware generated ECC. It shows how to use the various FlashCTL APIs to write different data sizes to flash memory with proper error handling.

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

This example demonstrates the following FlashCTL operations related to writing data of various sizes to flash:

1. Programming Data of Different Sizes with Hardware-Generated ECC:
   - 8-bit data using DL_FlashCTL_programMemoryFromRAM8WithECCGenerated
   - 16-bit data using DL_FlashCTL_programMemoryFromRAM16WithECCGenerated
   - 32-bit data using DL_FlashCTL_programMemoryFromRAM32WithECCGenerated
   - 64-bit data using DL_FlashCTL_programMemoryFromRAM64WithECCGenerated
   - 96-bit data using DL_FlashCTL_programMemoryFromRAM96WithECCGenerated
   - 128-bit data using DL_FlashCTL_programMemoryFromRAM128WithECCGenerated

2. Block Programming:
   - Programming multiple 64-bit words using DL_FlashCTL_programMemoryBlockingFromRAM64WithECCGenerated
   - This demonstrates how to efficiently program multiple words in sequence

3. Flash Configuration:
   - Setting flash wait states using DL_FlashCTL_setWaitState
   - This shows how to configure the flash controller for optimal performance

The example uses the following FlashCTL APIs from dl_flashctl.h:
- DL_FlashCTL_executeClearStatus: Clears the status register before operations
- DL_FlashCTL_unprotectSector: Unprotects a sector for erase/program operations
- DL_FlashCTL_eraseMemoryFromRAM: Erases a sector of memory
- DL_FlashCTL_programMemoryFromRAM8/16/32/64/96/128WithECCGenerated: Programs data of various sizes with hardware-generated ECC
- DL_FlashCTL_programMemoryBlockingFromRAM64WithECCGenerated: Programs multiple 64-bit words in sequence
- DL_FlashCTL_setWaitState: Sets the flash wait states for optimal performance
- DL_FlashCTL_waitForCmdDone: Waits for a command to complete

The example demonstrates proper error handling for flash operations, checking the status after each operation and setting appropriate error flags:
- If all operations succeed, the PASS_LED (LED2 Green) is turned on
- If any operation fails, the FAIL_LED (LED2 Red) is turned on and a breakpoint is triggered

This example highlights the flexibility of the flash controller in programming data of various sizes with hardware-generated ECC, which is important for ensuring data integrity in flash memory.
