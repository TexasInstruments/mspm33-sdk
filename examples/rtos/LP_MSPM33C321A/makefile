MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../..)
include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

# Find all subdirectories
RTOS-TICLANG-SUBDIRS = $(wildcard */*/freertos/ticlang/. */*/*/freertos/ticlang/.)

RTOS-SUBDIRS =
ifneq ($(TICLANG_ARMCOMPILER),)
	RTOS-SUBDIRS += $(RTOS-TICLANG-SUBDIRS)
endif

# Separate TrustZone secure/nonsecure subdirs to enforce build order:
# secure must complete before nonsecure (implib.o dependency)
SECURE-SUBDIRS    = $(filter %_secure/ticlang/., $(RTOS-SUBDIRS))
NONSECURE-SUBDIRS = $(filter %_nonsecure/ticlang/., $(RTOS-SUBDIRS))

.PHONY: rtos $(RTOS-SUBDIRS)

all: rtos

rtos: $(RTOS-SUBDIRS)

# Non-secure projects depend on secure projects (need implib.o from secure build)
$(NONSECURE-SUBDIRS): $(SECURE-SUBDIRS)

$(RTOS-SUBDIRS):
	@ $(MAKE) -C $@

CLEAN-RTOS-SUBDIRS = $(addprefix clean-,$(RTOS-SUBDIRS))

.PHONY: clean-rtos $(CLEAN-RTOS-SUBDIRS)

clean: clean-rtos

clean-rtos: $(CLEAN-RTOS-SUBDIRS)

$(CLEAN-RTOS-SUBDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean
