## Example Summary

This code example demonstrates how to use the ML-KEM-768 key encapsulation 
mechanism to perform encapsulation and generate the
associated ciphertext and shared secret.

The NIST ACVP FIPS 203 ML-KEM test case ID #26 is used here
to demonstate the encap computation.  The routine is expected
to return a success status and the actual (computed) cipher text and
shared secret value are expected to match the test case result values.

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
