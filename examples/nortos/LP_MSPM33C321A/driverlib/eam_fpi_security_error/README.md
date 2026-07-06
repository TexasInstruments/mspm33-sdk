## Example Summary

This example demonstrates an EAM (Error Aggregator Module) FPI (Flash Programming Interface) security error.
In Flash memory operations, before erasing any address, we need to unprotect that address. In this example, 
we try to erase an address (0x30000000) that is outside the valid flash memory range, which causes an "FPI ILL_ADDRESS" security error.
The example shows how to handle this error and check the EAM registers for the error address (at which the error occurred), 
MSTID (FPI), and error type flag (security error).

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| GPIOC | PC27 | Standard Output (Green LED) |
| GPIOC | PC26 | Standard Output (Red LED) |
| EAM |  |  |
| FLASHCTL |  |  |
| GSC |  |  |
| SYSCTL |  |  |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad information, including user guide and hardware files.

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PC27 | GPIOC | PASS_LED | N/A | <ul><li>PC27 is connected to LED2 Green<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PC26 | GPIOC | FAIL_LED | N/A | <ul><li>PC26 is connected to LED2 Red<br><ul><li>LED2 is a tri-color LED (Red, Green, Blue)</ul></ul> |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging</ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging</ul> |

### Low-Power Recommendations
TI recommends to terminate unused pins by setting the corresponding functions to
GPIO and configure the pins to output low or input with internal
pullup/pulldown resistor.

SysConfig allows developers to easily configure unused pins by selecting **Board**→**Configure Unused Pins**.

For more information about jumper configuration to achieve low-power using the
MSPM33 LaunchPad, please visit the [LP-MSPM33C321A User's Guide](https://www.ti.com/lit/pdf/slau962).

## Example Usage

The example performs the following operations:
1. Enables the Non-Maskable Interrupt (NMI) for security errors
2. Acquires the flash semaphore using DL_GSC_acquireFlashSemaphore
3. Unprotects a sector in main memory at address 0x00000000
4. Attempts to erase a sector at address 0x30000000, which is outside the valid flash memory range
5. This invalid address causes an "FPI ILL_ADDRESS" security error, triggering the NMI handler
6. The NMI handler checks if the error is an FPI_ILLADDR error, and if so, turns on the green LED

Compile, load and run the example. The green LED should be off at the beginning of
the program execution and will turn on when the security error is detected and handled.
The program will hit a breakpoint after the error occurs, allowing you to inspect the EAM registers
to see the error address, MSTID (FPI), and error type flag (security error).
