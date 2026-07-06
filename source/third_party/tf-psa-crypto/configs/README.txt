This directory contains example configuration files.

The examples are generally focused on a particular usage case (eg, support for
a restricted number of cryptographic mechanisms) and aim at minimizing resource
usage for this target. They can be used as a basis for custom configurations.

These files are complete replacements for the default crypto_config.h. To use
one of them, you can pick one of the following methods:

1. Replace the default file include/psa/crypto_config.h with the chosen one.

2. Use the TF_PSA_CRYPTO_CONFIG_FILE CMake option. For example, to build
   out-of-tree with the crypto-config-ccm-aes-sha256.h configuration file:

   cmake -DTF_PSA_CRYPTO_CONFIG_FILE="configs/crypto-config-ccm-aes-sha256.h" \
         -B build-ccmonly
   cmake --build build-ccmonly

The second method also works if you want to keep your custom configuration
file outside the TF-PSA-Crypto tree.

Configuration files
-------------------

* crypto-config-ccm-aes-sha256.h
  Minimal configuration supporting only CCM-AES and SHA-256.

* crypto-config-symmetric-only.h
  Configuration with symmetric cryptography only (no asymmetric algorithms).

* crypto-config-m33-baremetal.h
  Configuration optimized for bare-metal Cortex-M33 (ARMv8-M) embedded systems.
  This configuration:
  - Disables features requiring OS support (filesystem I/O, time functions)
  - Disables built-in entropy (requires user-provided entropy source)
  - Uses static key slots to avoid dynamic memory allocation
  - Enables size optimizations (smaller SHA implementations, fewer AES tables)
  - Supports ECC (ECDH, ECDSA with P-256), AES, SHA-2, HMAC, HKDF
  - Can be used with the TI ARM Clang toolchain via cmake/toolchain-tiarmclang-m33.cmake

  Example build command:
    cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-tiarmclang-m33.cmake \
          -DTF_PSA_CRYPTO_CONFIG_FILE="configs/crypto-config-m33-baremetal.h" \
          -DENABLE_TESTING=Off \
          -B build-m33
    cmake --build build-m33
