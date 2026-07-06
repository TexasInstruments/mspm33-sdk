# TF-M Empty Example for LP_MSPM33C321A

## Overview

This example demonstrates how to build and flash a minimal **Trusted Firmware-M (TF-M)** application on the LP_MSPM33C321A LaunchPad. TF-M is an open-source reference implementation of the **PSA (Platform Security Architecture)** Firmware Framework for Arm Cortex-M33 devices with TrustZone.

### This Example

This example serves as a minimal **TF-M application**:

- **Secure side (tfm_s.bin)**: Complete TF-M firmware with default PSA services (Crypto, Storage, Attestation, and Platform) enabled
- **Non-Secure side (tfm_ns.bin)**: Minimal application that uses psa_framework_version() to confirm TF-M functionality

Use this as a **starting point** for building your own TF-M applications. The README explains how to extend it with PSA Crypto, Storage, and Attestation APIs.

---

## Memory Layout

The MSPM33C321A has 1 MB flash and 256 KB SRAM, partitioned between Secure and Non-Secure worlds:

| Region         | Address Range           | Size   | Security | Contents                     |
|----------------|-------------------------|--------|----------|------------------------------|
| S Flash        | 0x1000_0000 - 0x1007_FFFF | 512 KB | Secure   | TF-M firmware (tfm_s.bin)    |
| NS Flash       | 0x0008_0000 - 0x000F_FFFF | 512 KB | NS       | NS application (tfm_ns.bin)  |
| S SRAM         | 0x3000_0000 - 0x3001_FFFF | 128 KB | Secure   | TF-M runtime data            |
| NS SRAM        | 0x2002_0000 - 0x2003_FFFF | 128 KB | NS       | NS application data          |
| Data Flash     | 0x8000_0000 - 0x8000_7FFF | 32 KB  | Secure   | ITS, PS, NV counters         |

**Note**: Flash addresses above use the **Non-Secure alias** (0x0008_0000) for NS programming tools. The Secure flash alias starts at 0x1000_0000.

---

## Prerequisites

Before building, ensure you have the following installed. For the full, authoritative list of
supported environments and versions, refer to the TF-M getting started guide bundled with the SDK:

> `source/third_party/trusted-firmware-m/docs/getting_started/tfm_getting_started.rst`

### 1. GNU Arm Embedded Toolchain
   - **Minimum version**: 12.2.Rel1 (as required by TF-M — see `tfm_getting_started.rst`)
   - **Download**: [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
   - Verify: `arm-none-eabi-gcc --version`

### 2. CMake (≥ 3.21)
   - **Download**: [cmake.org/download](https://cmake.org/download/)
   - Verify: `cmake --version`

### 3. MSP-SDK
   - This example is located at: `<SDK_ROOT>/examples/trusted-firmware-m/empty/LP_MSPM33C321A/`
   - TF-M source: `<SDK_ROOT>/source/third_party/trusted-firmware-m/`

### 4. Build Environment
   - **Windows**: CCS Terminal, Git Bash, or MSYS2 (for Unix-like shell commands)
   - **Linux/Mac**: Any bash-compatible shell

---

## Build Instructions

TF-M uses a **two-phase CMake build**:

1. **Phase 1 (SPE)**: Build the Secure Processing Environment (TF-M firmware)
2. **Phase 2 (NS)**: Build the Non-Secure application (your app)

### Phase 1: Build Secure Firmware (SPE)

Set up environment variables (adjust paths for your system):

```bash
# Path to MSP-SDK root
export SDK_ROOT=/home/user/ti/msp-sdk

# Path to GNU Arm toolchain prefix (directory + base name, without -gcc suffix)
# Adjust to your installation path. Examples:
#   Linux, standalone install:
#     export CROSS_COMPILE=/opt/arm-gnu-toolchain-12.3.rel1/bin/arm-none-eabi
#   Windows (Git Bash / CCS Terminal):
#     export CROSS_COMPILE="C:/ti/ccs/tools/compiler/<arm-gnu-toolchain-version>/bin/arm-none-eabi"
export CROSS_COMPILE=/path/to/arm-gnu-toolchain/bin/arm-none-eabi

# Build output directory
export BUILD_DIR=${SDK_ROOT}/tfm_build
```

Configure and build the SPE:

```bash
cmake -S ${SDK_ROOT}/source/third_party/trusted-firmware-m \
      -B ${BUILD_DIR}/spe \
      -DTFM_TOOLCHAIN_FILE=${SDK_ROOT}/source/third_party/trusted-firmware-m/toolchain_GNUARM.cmake \
      -DCROSS_COMPILE=${CROSS_COMPILE} \
      -DTI_SDK_PATH=${SDK_ROOT} \
      -DTFM_PLATFORM=ti/LP_MSPM33C321A \
      -DTFM_SYSTEM_PROCESSOR=cortex-m33 \
      -DTFM_SYSTEM_ARCHITECTURE=armv8-m.main \
      -DTFM_SYSTEM_DSP=OFF \
      -DCONFIG_TFM_ENABLE_FP=OFF \
      -DCMAKE_BUILD_TYPE=Release
```

Build and install:

```bash
cmake --build ${BUILD_DIR}/spe --target install -- -j$(nproc)
```

**Outputs** (in `${BUILD_DIR}/spe/bin/`):
- `tfm_s.axf` — Secure firmware ELF (with debug symbols)
- `tfm_s.bin` — Secure firmware binary (flash at **0x10000000**)
- `tfm_s.map` — Linker map file

**Installed files** (in `${BUILD_DIR}/spe/api_ns/`):
- `cmake/` — CMake modules and toolchain file for NS build
- `include/` — PSA API headers (`psa/client.h`, `psa/crypto.h`, etc.)
- `platform/` — Platform CMake modules (CPU flags, linker script wrappers, region definitions)
- `CMakeLists.txt` — NS project template

---

### Phase 2: Build Non-Secure Application (NS)

The NS build uses the `api_ns` install tree from Phase 1. Point CMake to the example NS app:

```bash
cmake -S ${SDK_ROOT}/examples/trusted-firmware-m/empty/LP_MSPM33C321A \
      -B ${BUILD_DIR}/ns \
      -DCONFIG_SPE_PATH=${BUILD_DIR}/spe/api_ns \
      -DTFM_TOOLCHAIN_FILE=${BUILD_DIR}/spe/api_ns/cmake/toolchain_ns_GNUARM.cmake \
      -DCROSS_COMPILE=${CROSS_COMPILE} \
      -DTI_SDK_PATH=${SDK_ROOT} \
      -DTFM_PLATFORM=ti/LP_MSPM33C321A \
      -DTFM_SYSTEM_PROCESSOR=cortex-m33 \
      -DTFM_SYSTEM_ARCHITECTURE=armv8-m.main \
      -DTFM_SYSTEM_DSP=OFF \
      -DCONFIG_TFM_ENABLE_FP=OFF \
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${BUILD_DIR}/ns/bin
```

Build:

```bash
cmake --build ${BUILD_DIR}/ns --target tfm_ns -- -j$(nproc)
```

**Outputs** (in `${BUILD_DIR}/ns/bin/`):
- `tfm_ns.axf` — NS application ELF (with debug symbols)
- `tfm_ns.bin` — NS application binary (flash at **0x00080000**)

---

## Flashing the Binaries

You must flash **both** binaries to the device.

### Using CCS

1. Launch CCS and create a target configuration using **MSPM33C321A_XDS110.ccxml**
2. Load `tfm_s.bin`:
   - **Run → Load → Load Program**
   - Select `${BUILD_DIR}/spe/bin/tfm_s.bin`
   - Set **Address: 0x10000000**
   - Erase: **Required Sectors Only**
3. Load `tfm_ns.bin`:
   - **Run → Load → Load Program**
   - Select `${BUILD_DIR}/ns/bin/tfm_ns.bin`
   - Set **Address: 0x00080000**
   - Erase: **None** (already erased)
4. Reset the device: **Run → Reset → CPU Reset**

---

## Project-less Debugging in CCS

For debugging with full symbol support (variable inspection, source-level stepping), it is highly recommended to load the **.axf** (ELF) files instead of the raw **.bin** files.

### Loading Sequence

CCS updates the core registers (PC and SP) based on the *latest* loaded binary, the following sequence is required to ensure the device boots correctly from the Secure side:

1. **Load Non-Secure (NS) ELF**:
   - **Run → Load → Load Program**
   - Select `${BUILD_DIR}/ns/bin/tfm_ns.axf`
   - Set **Address: 0x00080000**
   - Erase: **None**

2. **Load Secure (SPE) ELF**:
   - **Run → Load → Load Program**
   - Select `${BUILD_DIR}/spe/bin/tfm_s.axf`
   - Set **Address: 0x10000000**
   - Erase: **Required Sectors Only**

**Why this order?** Loading the SPE second ensures the CPU starts at the Secure entry point (as required for TF-M boot). The debugger will overwrite the PC/SP originally set by the NS load with the Secure entry point values.

---

## Expected UART Output

Connect a serial terminal to the LaunchPad UART (115200 baud, 8N1). After flashing and reset, you should see:

```
--- TF-M Non-Secure Application ---
PSA Framework version: 0x00000101 (v1.1)
TF-M is running successfully!

Entering infinite loop (application running)...
```

The key line is **`PSA Framework version: 0x00000101`** — this confirms:
- TF-M Secure firmware is running
- Non-Secure world can call PSA APIs
- TrustZone partitioning is operational

---

## Customizing the Example

### Adding PSA Crypto API Calls

To use PSA Crypto from the NS application:

1. Include the header:
   ```c
   #include "psa/crypto.h"
   ```

2. Initialize PSA Crypto:
   ```c
   psa_status_t status = psa_crypto_init();
   if (status != PSA_SUCCESS) {
       printf("psa_crypto_init failed: %d\n", status);
   }
   ```

3. Call crypto APIs (example: hash):
   ```c
   psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;
   uint8_t hash[32];
   size_t hash_len;

   status = psa_hash_setup(&op, PSA_ALG_SHA_256);
   status = psa_hash_update(&op, data, data_len);
   status = psa_hash_finish(&op, hash, sizeof(hash), &hash_len);
   ```

4. Rebuild the NS application (Phase 2 only):
   ```bash
   cmake --build ${BUILD_DIR}/ns --target tfm_ns -- -j$(nproc)
   ```

5. Flash only `tfm_ns.bin` at 0x00080000 (no need to reflash tfm_s.bin)

### Adding PSA Storage API Calls

Internal Trusted Storage (ITS) and Protected Storage (PS) are enabled by default:

```c
#include "psa/storage_common.h"
#include "psa/internal_trusted_storage.h"

psa_storage_uid_t uid = 42;
uint8_t data[] = "Hello, TF-M!";
size_t data_len = sizeof(data);

status = psa_its_set(uid, data_len, data, PSA_STORAGE_FLAG_NONE);
if (status != PSA_SUCCESS) {
    printf("psa_its_set failed: %d\n", status);
}
```

### Adding More Secure Services

The default configuration enables Crypto, Internal Trusted Storage (ITS), Protected Storage (PS), Initial Attestation, and Platform services. To enable or disable additional partitions, modify the SPE build:

1. Edit the cmake command in Phase 1, adding partition flags (e.g., to disable Protected Storage):
   ```bash
   -DTFM_PARTITION_PROTECTED_STORAGE=OFF
   ```

2. Rebuild both SPE and NS (both phases)

See `source/third_party/trusted-firmware-m/platform/ext/target/ti/LP_MSPM33C321A/config.cmake` for all available partition options.

---

## Debugging

### Debugging with CCS

1. Add debug symbols for both worlds:
   - **Secure**: Load `${BUILD_DIR}/spe/bin/tfm_s.axf`
   - **Non-Secure**: Load `${BUILD_DIR}/ns/bin/tfm_ns.axf`

2. Set breakpoints in NS code (`main_ns.c:main`)


---

## Startup and Linker Script

The reference files in `gcc/nortos/` are provided for **IDE browsing only**. The CMake build does **not** use them. Instead, the startup code and linker script are sourced automatically from the SPE install tree:

- **Startup**: `${CONFIG_SPE_PATH}/platform/Device/Source/startup_mspm33c321a_s.c`
- **Linker script**: `${CONFIG_SPE_PATH}/platform/linker_scripts/tfm_common_ns.ld`

These files **must come from the SPE build** because the NS memory map must exactly match what the Secure side configured. To change the memory layout, modify the Phase 1 (SPE) CMake options and rebuild — the NS side will automatically pick up the updated layout.

---
