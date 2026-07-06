# ===========================================================================
# Root Makefile — mbed-tls Cortex-M33 bare-metal build
#
# Delegates all compilation to library/Makefile.
# All variables are overridable from the command line or from TI CCS
# project build settings (Project Properties → Build → Make).
#
# Targets:
#   make / make lib        — build static libraries
#   make generated_files   — run code-generation scripts only (needs Python+Perl)
#   make clean             — remove compiled objects and .a files
#   make neat              — clean + remove generated source files
#
# Common overrides:
#   make CC=tiarmclang AR=tiarmar            (TI Clang toolchain)
#   make OPT_FLAGS=-O2                       (change optimisation)
#   make CPU_FLAGS="-mcpu=cortex-m33 -mthumb -mfloat-abi=soft"  (soft float)
#   make MBEDTLS_CONFIG_FILE=/abs/path/my_config.h
#   make GEN_FILES=no                        (skip code generation - pre-generated files must exist)
# ===========================================================================

MBEDTLS_PATH := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# ---------------------------------------------------------------------------
# Toolchain
# Default: GCC ARM Embedded.  Override for TI Clang: CC=tiarmclang AR=tiarmar
# ---------------------------------------------------------------------------
CC  ?= arm-none-eabi-gcc
AR  ?= arm-none-eabi-ar

# ---------------------------------------------------------------------------
# Cortex-M33 CPU flags (ARMv8-M Mainline, hard-float FPU)
# For targets without FPU: CPU_FLAGS = -mcpu=cortex-m33 -mthumb -mfloat-abi=soft
# ---------------------------------------------------------------------------
CPU_FLAGS ?= -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard

# ---------------------------------------------------------------------------
# Optimisation — -Os (size) is preferred for embedded; use -O2 for speed
# ---------------------------------------------------------------------------
OPT_FLAGS ?= -Os

# ---------------------------------------------------------------------------
# Config files
#   MBEDTLS_CONFIG_FILE       — TLS/X.509 layer  (configs/ in this repo)
#   TF_PSA_CRYPTO_CONFIG_FILE — PSA crypto layer  (tf-psa-crypto/configs/)
#                               Leave empty to use the library default.
# ---------------------------------------------------------------------------
MBEDTLS_CONFIG_FILE       ?= $(MBEDTLS_PATH)/configs/config-m33-baremetal.h
TF_PSA_CRYPTO_CONFIG_FILE ?= $(MBEDTLS_PATH)/tf-psa-crypto/configs/crypto-config-m33-baremetal.h

# ---------------------------------------------------------------------------
# Assemble the -D flags for the config files.
# These are kept separate from CFLAGS so they are always present even when
# OPT_FLAGS / CPU_FLAGS are overridden from CCS.
# ---------------------------------------------------------------------------
_CONFIG_DEFS := -DMBEDTLS_CONFIG_FILE=\"$(MBEDTLS_CONFIG_FILE)\"
ifneq ($(TF_PSA_CRYPTO_CONFIG_FILE),)
_CONFIG_DEFS += -DTF_PSA_CRYPTO_CONFIG_FILE=\"$(TF_PSA_CRYPTO_CONFIG_FILE)\"
endif

# ---------------------------------------------------------------------------
# CFLAGS — fully assembled from the above components.
# To add project-specific flags without replacing everything, use EXTRA_CFLAGS.
#   e.g.  make EXTRA_CFLAGS="-DMY_DEFINE=1"
# ---------------------------------------------------------------------------
CFLAGS = $(OPT_FLAGS) -std=c99 $(CPU_FLAGS) $(_CONFIG_DEFS) $(EXTRA_CFLAGS)

# ---------------------------------------------------------------------------
# Warning flags — -Werror is intentionally omitted for cross-compilation
# (toolchain-specific warnings differ from the host GCC).
# ---------------------------------------------------------------------------
WARNING_CFLAGS ?= -Wall -Wextra -Wmissing-prototypes

# ---------------------------------------------------------------------------
# Bare-metal: no threading, always build static libraries only
# ---------------------------------------------------------------------------
THREADING =
SHARED    =

# ---------------------------------------------------------------------------
# Code generation (default: enabled — requires Python 3 and Perl).
# Set GEN_FILES=no if cross-compiling in an environment without these tools
# and the generated files (error.c, version_features.c, etc.) already exist.
# ---------------------------------------------------------------------------
GEN_FILES ?= yes

# ===========================================================================
# Targets
# ===========================================================================
.PHONY: all lib generated_files clean neat

all: lib

lib:
	$(MAKE) -C $(MBEDTLS_PATH)/library \
	    MBEDTLS_PATH=$(MBEDTLS_PATH) \
	    CC="$(CC)" \
	    AR="$(AR)" \
	    CFLAGS="$(CFLAGS)" \
	    WARNING_CFLAGS="$(WARNING_CFLAGS)" \
	    GEN_FILES=$(GEN_FILES) \
	    static

generated_files:
	$(MAKE) -C $(MBEDTLS_PATH)/library \
	    MBEDTLS_PATH=$(MBEDTLS_PATH) \
	    GEN_FILES=$(GEN_FILES) \
	    generated_files

clean:
	$(MAKE) -C $(MBEDTLS_PATH)/library clean

neat:
	$(MAKE) -C $(MBEDTLS_PATH)/library neat
