## Example Summary

This code example demonstrates how to use the ML-KEM-1024 key encapsulation 
mechanism to perform a key generation, encapsulation, and decapsulation sequence.

The entropy source (TRNG) is used to seed a DRBG which is subsequently used to
generate the random seed values for ML-KEM-1024.  From the seed values, ML-KEM-1024
encapsulation and decapsulation keys are first generated.  Then, the encapsulation key is used
with additional random seed data to generate a ciphertext and shared secret.  Finally, the
decapsulation function is used with the decapsulation key to take the ciphertext and generate
the same shared secret.

In the end, the shared secret values are expected to match for the example to pass.

This example combines encapsulation and decapsulation into a single code example to show the
sequence.  In a real world application, ML-KEM-1024 would be used by two processors/systems
to agree to a shared secret over an unsecure channel, where on side performs encapsulation and
the other side performs decapsulation, and the ciphertext is shared across the unsecure channel.

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
