MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../..)
include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

# Find all subdirectories
NORTOS-TICLANG-SUBDIRS = $(filter-out $(wildcard driverlib/empty_driverlib_library/*/.),$(wildcard */*/ticlang/. */*/*/ticlang/.))

NORTOS-SUBDIRS =
ifneq ($(TICLANG_ARMCOMPILER),)
	NORTOS-SUBDIRS += $(NORTOS-TICLANG-SUBDIRS)
endif

# Separate TrustZone secure/nonsecure subdirs to enforce build order:
# secure must complete before nonsecure (implib.o dependency)
SECURE-SUBDIRS    = $(filter %_secure/ticlang/., $(NORTOS-SUBDIRS))
NONSECURE-SUBDIRS = $(filter %_nonsecure/ticlang/., $(NORTOS-SUBDIRS))

.PHONY: nortos $(NORTOS-SUBDIRS)

all: nortos

nortos: $(NORTOS-SUBDIRS)

# Non-secure projects depend on secure projects (need implib.o from secure build)
$(NONSECURE-SUBDIRS): $(SECURE-SUBDIRS)

$(NORTOS-SUBDIRS):
	@ $(MAKE) -C $@

CLEAN-NORTOS-SUBDIRS = $(addprefix clean-,$(NORTOS-SUBDIRS))

.PHONY: clean-nortos $(CLEAN-NORTOS-SUBDIRS)

clean: clean-nortos

clean-nortos: $(CLEAN-NORTOS-SUBDIRS)

$(CLEAN-NORTOS-SUBDIRS):
	@ $(MAKE) -C $(subst clean-,,$@) clean
