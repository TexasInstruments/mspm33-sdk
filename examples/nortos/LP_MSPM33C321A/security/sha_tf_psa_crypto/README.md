## Example Summary

This example demonstrates SHA-256 hashing using the TF-PSA-Crypto library PSA
Crypto API on the LP-MSPM33C321A LaunchPad.

The example:
1. Computes SHA-256 of the string "Hello World!" using `psa_hash_compute()`
2. Compares the result against the known expected hash
3. Lights USER_LED_3 (Green) on success, or USER_LED_2 (Red) on failure

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
cd <SDK>/examples/nortos/LP_MSPM33C321A/security/sha_tf_psa_crypto/ticlang

make TICLANG_ARMCOMPILER=<path/to/ti-cgt-armllvm_X.X.X.LTS> \
     SYSCONFIG_TOOL=<path/to/sysconfig_cli.sh>
```

**Example (Linux):**
```bash
make TICLANG_ARMCOMPILER=/home/user/ti/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS \
     SYSCONFIG_TOOL=/home/user/ti/ccs/utils/sysconfig_1.27.1/sysconfig_cli.sh
```

On success, `sha_tf_psa_crypto.out` is generated in the `ticlang/` directory.

### Flash and Debug

1. Open CCS and connect the LP-MSPM33C321A LaunchPad via USB
2. Go to **Run → Load → Load Program** and select `sha_tf_psa_crypto.out`
3. Run the program and observe the LEDs

## Example Usage

After loading the program:

- **USER_LED_3 (Green)** lights → SHA-256 hash matched the expected value.
- **USER_LED_2 (Red)** lights → hash computation failed or result did not match.
