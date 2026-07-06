## Example Summary

This example demonstrates AES-128-CTR (Counter Mode) encryption and decryption
using the TF-PSA-Crypto library PSA Crypto API on the LP-MSPM33C321A LaunchPad,
with hardware acceleration via the on-chip AESADV peripheral.

Test vectors are from NIST SP 800-38A, Section F.5.1 (CTR-AES128).

The example:
1. Initializes the TRNG for PSA Crypto entropy
2. Imports a 128-bit AES key
3. Encrypts 64 bytes of plaintext with AES-CTR using `psa_cipher_encrypt()`
4. Verifies the ciphertext against the known NIST test vectors
5. Decrypts the ciphertext and verifies the result matches the original plaintext
6. Lights USER_LED_3 (Green) on success, or USER_LED_2 (Red) on any failure

## Peripherals & Pin Assignments

| Peripheral | Pin  | Function            |
|------------|------|---------------------|
| GPIOA      | PA2  | USER_LED_1 (Blue)   |
| GPIOC      | PC26 | USER_LED_2 (Red)    |
| GPIOC      | PC27 | USER_LED_3 (Green)  |
| DEBUGSS    | PA20 | Debug Clock (SWCLK) |
| DEBUGSS    | PA19 | Debug Data In/Out (SWDIO) |

## BoosterPacks, Board Resources & Jumper Settings

Visit [LP_MSPM33C321A](https://www.ti.com/tool/LP-MSPM33C321A) for LaunchPad
information, including user guide and hardware files.

## Building the Example

> **Note:** This example does not support CCS project import. Build using
> `make` and load the resulting `.out` file into CCS for flashing and debugging.

### Prerequisites

- **TI ARM Clang toolchain** — bundled with CCS at
  `<CCS_INSTALL_PATH>/tools/compiler/ti-cgt-armllvm_<version>`
- **SysConfig CLI** — bundled with CCS at
  `<CCS_INSTALL_PATH>/utils/sysconfig_<version>/sysconfig_cli.sh` (Linux) or
  `sysconfig_cli.bat` (Windows)

### Build

Navigate to the example's `ticlang/` directory and run:

```bash
cd <SDK>/examples/nortos/LP_MSPM33C321A/security/aes_ctr_tf_psa_crypto/ticlang

make TICLANG_ARMCOMPILER=<path/to/ti-cgt-armllvm_X.X.X.LTS> \
     SYSCONFIG_TOOL=<path/to/sysconfig_cli.sh>
```

**Example (Linux):**
```bash
make TICLANG_ARMCOMPILER=/home/user/ti/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS \
     SYSCONFIG_TOOL=/home/user/ti/ccs/utils/sysconfig_1.27.1/sysconfig_cli.sh
```

On success, `aes_ctr_tf_psa_crypto.out` is generated in the `ticlang/` directory.

### Flash and Debug

1. Open CCS and connect the LP-MSPM33C321A LaunchPad via USB
2. Go to **Run → Load → Load Program** and select `aes_ctr_tf_psa_crypto.out`
3. Run the program and observe the LEDs

## Example Usage

After loading the program:

- **USER_LED_3 (Green)** lights → AES-CTR encrypt/decrypt passed, ciphertext matches NIST SP 800-38A F.5.1 test vectors.
- **USER_LED_2 (Red)** lights → a PSA Crypto API call failed, or the ciphertext did not match the expected NIST vectors.
