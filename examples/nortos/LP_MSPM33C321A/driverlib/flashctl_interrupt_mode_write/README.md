## Example Summary

This example demonstrates non-blocking (interrupt-driven) flash operations on
MSPM33C321A. It erases a 2KB sector at address 0x3000, then programs the entire
sector word (128-bit / 16-byte) by word from 0x3000 to 0x3800 using the
FLASHCTL CMDDONE interrupt. The next flash operation is always triggered from
the FLASHCTL_IRQHandler, keeping main() free while operations are in progress.

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
LED2 Green (PASS_LED) will turn on after all flash operations complete successfully.
LED2 Red (FAIL_LED) will turn on if any operation fails.

## Application Design Details

This example demonstrates interrupt-driven (non-blocking) flash operations using
the FLASHCTL CMDDONE interrupt on MSPM33C321A.

**Operation sequence:**

1. Erase the 2KB sector at 0x3000 (non-blocking, triggered from main)
2. On CMDDONE interrupt: program the first 128-bit word at 0x3000
3. On each subsequent CMDDONE interrupt: program the next 128-bit word until
   the entire sector (0x3000–0x37FF, 128 words) is programmed

**APIs used:**

- `DL_FlashCTL_executeClearStatus`: Clears the status register before operations
- `DL_FlashCTL_enableInterrupt` / `DL_FlashCTL_disableInterrupt`: Enable/disable CMDDONE interrupt
- `DL_FlashCTL_clearInterruptStatus`: Clears the CMDDONE interrupt flag in the ISR
- `DL_FlashCTL_unprotectSector`: Unprotects a sector before each erase/program
- `DL_FlashCTL_eraseMemory`: Erases a sector (non-blocking, no FromRAM)
- `DL_FlashCTL_programMemory128WithECCGenerated`: Programs a 128-bit word (non-blocking)
- `DL_FlashCTL_getCommandStatus`: Checks pass/fail status of the last command

main() sleeps with `__WFI()` while all flash operations proceed through the
interrupt handler state machine. On completion, PASS_LED or FAIL_LED is lit and
a breakpoint is hit.
