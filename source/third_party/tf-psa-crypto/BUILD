package(default_visibility = ["//visibility:public"])

# Core PSA crypto layer: key management, slot management, storage, client API
filegroup(
    name = "core_srcs",
    srcs = glob(["core/*.c"]),
)

# Builtin software crypto drivers. aesni.c and aesce.c excluded as they are meant to
# accelerate operations for x86 and ARMv8-A, which is not applicable for M33
filegroup(
    name = "builtin_srcs",
    srcs = glob(
        ["drivers/builtin/src/*.c"],
        exclude = [
            "drivers/builtin/src/aesni.c",
            "drivers/builtin/src/aesce.c",
        ],
    ),
)

filegroup(
    name = "utilities_srcs",
    srcs = glob(["utilities/*.c"]),
)

filegroup(
    name = "platform_srcs",
    srcs = glob(["platform/*.c"]),
)

filegroup(
    name = "extras_srcs",
    srcs = glob(["extras/*.c"]),
)

filegroup(
    name = "m33_srcs",
    srcs = [
        ":core_srcs",
        ":builtin_srcs",
        ":utilities_srcs",
        ":platform_srcs",
        ":extras_srcs",
    ],
)

filegroup(
    name = "public_hdrs",
    srcs = glob(["include/**/*.h"]),
)

filegroup(
    name = "builtin_hdrs",
    srcs = glob([
        "drivers/builtin/include/**/*.h",
        "drivers/builtin/src/*.h",
    ]),
)

filegroup(
    name = "core_hdrs",
    srcs = glob(["core/*.h"]),
)

filegroup(
    name = "config_hdrs",
    srcs = glob([
        "configs/*.h",
        "configs/**/*.h",
    ]),
)

filegroup(
    name = "utilities_hdrs",
    srcs = glob(["utilities/*.h"]),
)

filegroup(
    name = "dispatch_hdrs",
    srcs = glob(["dispatch/*.h"]),
)

filegroup(
    name = "platform_hdrs",
    srcs = glob(["platform/*.h"]),
)

filegroup(
    name = "extras_hdrs",
    srcs = glob(["extras/*.h"]),
)

filegroup(
    name = "m33_hdrs",
    srcs = [
        ":public_hdrs",
        ":builtin_hdrs",
        ":core_hdrs",
        ":config_hdrs",
        ":utilities_hdrs",
        ":dispatch_hdrs",
        ":platform_hdrs",
        ":extras_hdrs",
    ],
)

filegroup(
    name = "mspm33_sha_srcs",
    srcs = ["drivers/mspm33/sha/ti_mspm33_sha_driver.c"],
)

filegroup(
    name = "mspm33_sha_hdrs",
    srcs = glob(["drivers/mspm33/include/*.h"]),
)

filegroup(
    name = "mspm33_aes_srcs",
    srcs = ["drivers/mspm33/aes/ti_mspm33_aes_driver.c"],
)

filegroup(
    name = "mspm33_aes_hdrs",
    srcs = glob(["drivers/mspm33/include/ti_mspm33_aes*.h"]),
)

arm_cc_generic_library(
    name = "tfpsacrypto_m33_baremetal",
    libname = "libtfpsacrypto",
    arch = "m33",
    compilers = ["ticlang"],
    srcs = [
        ":m33_srcs",
        ":mspm33_sha_srcs",
        ":mspm33_aes_srcs",
    ],
    hdrs = [
        ":m33_hdrs",
        ":mspm33_sha_hdrs",
        ":mspm33_aes_hdrs",
    ],
    includes = [
        "include",
        "drivers/builtin/include",
        "drivers/builtin/src",
        "utilities",
        "core",
        "configs",
        "dispatch",
        "platform",
        "extras",
        "drivers/mspm33/include",
    ],
    defines = [
        "TF_PSA_CRYPTO_CONFIG_FILE=<crypto-config-m33-baremetal.h>",
        "CRYPTO_HW_USE_MSPM33_SHA",
        "CRYPTO_HW_USE_MSPM33_AES",
    ],
    copts = [
        "-Oz",
        "-D__MSPM33C321A__",
        "-DCRYPTO_HW_USE_MSPM33_TRNG=1",
    ],
    deps = [
        "//source/ti/driverlib:mspm33c321a_lib",
    ],
    visibility = ["//visibility:public"],
)

arm_cc_generic_library(
    name = "tfpsacrypto_m33_symmetric",
    libname = "libtfpsacrypto",
    arch = "m33",
    compilers = ["ticlang"],
    srcs = [":m33_srcs"],
    hdrs = [":m33_hdrs"],
    includes = [
        "include",
        "drivers/builtin/include",
        "drivers/builtin/src",
        "utilities",
        "core",
        "configs",
        "dispatch",
        "platform",
        "extras",
    ],
    defines = [
        "TF_PSA_CRYPTO_CONFIG_FILE=<crypto-config-symmetric-only.h>",
    ],
    copts = [
        "-Oz",
        "-D__MSPM33C321A__",
        "-DCRYPTO_HW_USE_MSPM33_TRNG=1",
    ],
    visibility = ["//visibility:public"],
)

arm_cc_generic_library(
    name = "tfpsacrypto_m33_ccm_aes_sha256",
    libname = "libtfpsacrypto",
    arch = "m33",
    compilers = ["ticlang"],
    srcs = [":m33_srcs"],
    hdrs = [":m33_hdrs"],
    includes = [
        "include",
        "drivers/builtin/include",
        "drivers/builtin/src",
        "utilities",
        "core",
        "configs",
        "dispatch",
        "platform",
        "extras",
    ],
    defines = [
        "TF_PSA_CRYPTO_CONFIG_FILE=<crypto-config-ccm-aes-sha256.h>",
    ],
    copts = [
        "-Oz",
        "-D__MSPM33C321A__",
        "-DCRYPTO_HW_USE_MSPM33_TRNG=1",
    ],
    visibility = ["//visibility:public"],
)

arm_cc_generic_library(
    name = "tfpsacrypto_m33_medium",
    libname = "libtfpsacrypto",
    arch = "m33",
    compilers = ["ticlang"],
    srcs = [":m33_srcs"],
    hdrs = [":m33_hdrs"],
    includes = [
        "include",
        "drivers/builtin/include",
        "drivers/builtin/src",
        "core",
        "configs/ext",
        "utilities",
        "dispatch",
        "platform",
        "extras",
    ],
    defines = [
        "TF_PSA_CRYPTO_CONFIG_FILE=<crypto_config_profile_medium.h>",
    ],
    copts = [
        "-Oz",
        "-D__MSPM33C321A__",
        "-DCRYPTO_HW_USE_MSPM33_TRNG=1",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "libs",
    srcs = [
        ":tfpsacrypto_m33_baremetal",
    ],
    visibility = ["//visibility:public"],
)

# Convenience filegroup of all source/header files, useful
# for IDE project generation and SDK packaging.
filegroup(
    name = "srcs",
    srcs = glob(["**"]) + [":libs"],
    visibility = ["//visibility:public"],
)