## Example Summary

This example demonstrates flash word (128-bit) programming with automatic hardware generated ECC as well as with manually provided ECC. It shows different approaches to programming flash memory with proper error correction code handling.

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
The application will stop at breakpoints so the user can verify the values in memory match those expected in the comments.
LED2 Green (PASS_LED) will turn on after all flash operations are completed successfully.

## Application Design Details

This example demonstrates the following FlashCTL operations related to programming with ECC:

1. Single Flash Word Programming with Hardware-Generated ECC:
   - Programs a single 128-bit flash word with hardware-generated ECC
   - Uses DL_FlashCTL_programMemoryFromRAM128WithECCGenerated
   - The flash controller automatically generates the appropriate ECC code

2. Multiple Flash Word Programming with Hardware-Generated ECC:
   - Programs multiple 128-bit flash words with hardware-generated ECC
   - Uses DL_FlashCTL_programMemoryBlockingFromRAM128WithECCGenerated
   - Demonstrates efficient programming of multiple words in sequence

3. Multiple Flash Word Programming with Manually Provided ECC:
   - Programs multiple 128-bit flash words with manually provided ECC codes
   - Uses DL_FlashCTL_enableOverrideHardwareGeneratedECC to disable hardware ECC generation
   - Uses DL_FlashCTL_programMemoryBlockingFromRAM128WithECCManual to program data with custom ECC
   - Demonstrates how to provide custom ECC codes for special use cases

The example uses the following FlashCTL APIs from dl_flashctl.h:
- DL_FlashCTL_unprotectSector: Unprotects a sector for erase/program operations
- DL_FlashCTL_eraseMemoryFromRAM: Erases a sector of memory
- DL_FlashCTL_programMemoryFromRAM128WithECCGenerated: Programs a single 128-bit word with hardware-generated ECC
- DL_FlashCTL_programMemoryBlockingFromRAM128WithECCGenerated: Programs multiple 128-bit words with hardware-generated ECC
- DL_FlashCTL_enableOverrideHardwareGeneratedECC: Disables hardware ECC generation
- DL_FlashCTL_programMemoryBlockingFromRAM128WithECCManual: Programs multiple 128-bit words with manually provided ECC
- DL_FlashCTL_disableOverrideHardwareGeneratedECC: Re-enables hardware ECC generation

The example demonstrates proper error handling for flash operations, checking the status after each operation and setting appropriate error flags:
- If all operations succeed, the PASS_LED (LED2 Green) is turned on
- If any operation fails, the FAIL_LED (LED2 Red) is turned on and a breakpoint is triggered

This example highlights the flexibility of the flash controller in programming data with both hardware-generated and manually provided ECC, which is important for ensuring data integrity in flash memory while also allowing for special use cases that require custom ECC handling.
