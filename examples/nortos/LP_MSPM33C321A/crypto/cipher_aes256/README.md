## Example Summary

This code example demonstrates how to use the AES single block function
to encrypt and decrypt a single block.

The NIST CAVP AES ECB 1.0 test ID #635 is used to demonstrate AES-256 encryption and decryption.
Note that input / output buffers must be 32-bit word aligned.

Note that this basic form of encryption is not secure on its own. The
same plaintext will always yield the same ciphertext.  This single block
ECB encryption may be used as a building block for more robust schemes
but should not be used on its own in most cases.

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
