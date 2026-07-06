## Example Summary

This example demonstrates how to write to the BCR and BSL configuration structures that are in NONMAIN Memory.

The BCR and BSL configuration data structures are both contained within a single flash sector in the NONMAIN
flash memory region.

Both the BCR and BSL configuration data structures are protected by individual 32-bit CRCs to improve security.
The application first calculates the 32-bit CRCs for both structures based on the user configuration values, and then sets the calculated CRC for each structure.

To successfully change any parameter in the boot configuration, the application must erase the entire NONMAIN sector. Then both the BCR and BSL configuration structures are re-programed with the given user configuration values.

The BCR and BSL configuration structures at application startup are configured with the default values. To update the values, the user should refer to boot_config.h to view the possible values that may be configured to BCR and BSL configuration structures.

*Note*: Please use caution before erasing and writing to NONMAIN memory.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (PASS_LED) |
| GPIOC | PC26 | Standard Output (FAIL_LED) |
| SYSCTL |  |  |
| FLASHCTL |  |  |
| CRC |  |  |
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
As NM config is being used in Syscfg, set the 'Erase method' in 'MSPM33 Flash Settings' to "Erase MAIN and NONMAIN memory" [Warning: Change it back to "Erase MAIN memory only" after the example use to avoid unnecessary modification of NONMAIN]. 
Compile, load and run the example.
The flash operations will start automatically when the example is run.
LED2 Green (PASS_LED) will blink after all flash operations are completed successfully.
The user can view the contents of memory to verify the results of the write operations.

## Application Design Details

This example demonstrates the following FlashCTL operations related to writing to NONMAIN memory:

1. BCR and BSL Configuration:
   - Defines default configurations for Boot Configuration Record (BCR) and Bootstrap Loader (BSL)
   - These configurations control device boot behavior, debug access, and security features

2. CRC Calculation:
   - Uses the CRCP peripheral to calculate 32-bit CRCs for both BCR and BSL configurations
   - The CRC values are used to validate the integrity of the configuration structures

3. NONMAIN Memory Operations:
   - Erasing the NONMAIN memory sector using DL_FlashCTL_eraseMemoryFromRAM
   - Programming the BCR configuration structure (336 bytes) to NONMAIN memory
   - Programming the BSL configuration structure (80 bytes) to NONMAIN memory

The example uses the following FlashCTL APIs from dl_flashctl.h:
- DL_FlashCTL_unprotectSector: Unprotects the NONMAIN memory sector for erase/program operations
- DL_FlashCTL_eraseMemoryFromRAM: Erases the NONMAIN memory sector
- DL_FlashCTL_programMemoryBlockingFromRAM64WithECCGenerated: Programs data to NONMAIN memory with hardware-generated ECC

The example also uses the CRCP peripheral to calculate 32-bit CRCs for data integrity:
- DL_CRCP_init: Initializes the CRCP peripheral with a 32-bit polynomial
- DL_CRCP_setSeed32: Sets the initial seed value for CRC calculation
- DL_CRCP_feedData8: Feeds data bytes into the CRC calculation
- DL_CRCP_getResult32: Gets the final 32-bit CRC result

This example highlights how to properly configure and write to NONMAIN memory regions, which is important for applications that need to modify boot configuration or other system settings stored in non-main flash memory.
