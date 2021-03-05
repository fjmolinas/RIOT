ifneq (,$(filter riotboot-rs,$(FEATURES_USED)))

.PHONY: riotboot-rs riotboot-rs/flash

RIOTBOOT_RS_FLASH_DIR = $(RIOTBASE)/bootloaders/riotboot

RIOTBOOT_RS_EXPORT_VARS ?= \
  RAM_START_ADDR \
  ROM_START_ADDR \
  RAM_LEN \
  RIOTBOOT_LEN \
  SLOT0_OFFSET \
  SLOT1_OFFSET \
  #

$(foreach var,$(RIOTBOOT_RS_EXPORT_VARS),$(call target-export-variables,riotboot-rs,$(var)))

riotboot-rs:
	@$(Q)CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/riotboot-rs

riotboot-rs/flash: riotboot-rs
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET) PATH="$(PATH)"\
		EXTERNAL_BOARD_DIRS="$(EXTERNAL_BOARD_DIRS)" BOARD="$(BOARD)"\
		DEBUG_ADAPTER_ID="$(DEBUG_ADAPTER_ID)" PROGRAMMER="$(PROGRAMMER)"\
		FLASHFILE="$(BINDIR)/riotboot-rs.bin"\
		$(MAKE) -C "$(RIOTBOOT_RS_FLASH_DIR)" --no-print-directory flash-only
endif # (,$(filter riotboot-rs,$(FEATURES_USED)))
