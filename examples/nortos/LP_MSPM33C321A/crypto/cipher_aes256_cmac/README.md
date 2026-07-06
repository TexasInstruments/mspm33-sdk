## Example Summary

This code example demonstrates how to use the AES cipher based message
authentication code (CMAC) function to generate and verify a message.

The NIST CAVP AES CMAC test vector #52 is used to demonstrate generation.
This example uses a MAC of length 5 to demonstrate that the MAC
can be smaller than 1 128-bit block.  However, TI recommends choosing
the largest MAC size (16 bytes / 128 bits) when possible.

This example uses the AES accelerator, but this module is configured 
and managed by the MSP-CRYPTO-LIB.  No configuration in SysConfig is
required for the AES.

## Peripherals & Pin Assignments

| Peripheral | Pin | Function |
| --- | --- | --- |
| DEBUGSS | PA20 | Debug Clock |
| DEBUGSS | PA19 | Debug Data In Out |

## BoosterPacks, Board Resources & Jumper Settings

| Pin | Peripheral | Function | LaunchPad Pin | LaunchPad Settings |
| --- | --- | --- | --- | --- |
| PA20 | DEBUGSS | SWCLK | N/A | <ul><li>PA20 is used by SWD during debugging<br><ul><li>`J14 15:16 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J14 15:16 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |
| PA19 | DEBUGSS | SWDIO | N/A | <ul><li>PA19 is used by SWD during debugging<br><ul><li>`J14 13:14 ON` Connect to XDS-110 SWDIO while debugging<br><li>`J14 13:14 OFF` Disconnect from XDS-110 SWDIO if using pin in application</ul></ul> |

## Example Usage
Compile, load and run the example.  Set break points at the trap loops at the end of main() to verify correct execution.
