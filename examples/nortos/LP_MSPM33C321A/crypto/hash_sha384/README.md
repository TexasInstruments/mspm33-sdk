## Example Summary

This example demonstrates how to use the SHA-384 hash function from the
MSP Crypto Library. SHA-384 is a member of the SHA-2 family of hash functions
defined in FIPS 180-4, producing a 384-bit (48-byte) message digest.

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

Compile, load, and run the example. The example computes the SHA-384 hash
of a test message using a NIST CAVP test vector (128-bit message length).

The computed hash is compared against the expected hash value. If they match,
the program enters an infinite loop at the pass condition. If the hash
computation fails or the hashes don't match, the program enters an infinite
loop at the error condition.

Use a debugger to verify the program reaches the pass condition loop.
