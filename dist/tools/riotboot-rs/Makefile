PKG_NAME=riotboot-rs
PKG_URL=https://github.com/future-proof-iot/riotboot-rs.git
PKG_VERSION=9d0f74701511b63bda7f272eb6065ed1d99013d7
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

ifneq (,$(filter cortex-m0%,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv6m-none-eabi
else ifneq (,$(filter cortex-m3,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv7m-none-eabi
else ifneq (,$(filter cortex-m4 cortex-m7,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv7em-none-eabi
else ifneq (,$(filter cortex-m4f cortex-m7f,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv7em-none-eabihf
else ifneq (,$(filter cortex-m23,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv8m.base-none-eabi
else ifneq (,$(filter cortex-m33,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv8m.main-none-eabi
else ifneq (,$(filter cortex-m33f,$(CPU_CORE)))
  CARGO_TARGET ?= thumbv8m.main-none-eabihf
else
  $(error unsupported CPU_CORE)
endif

ifneq (,$(filter cortex-m0%,$(CPU_CORE)))
CARGO_BUILD_FLAGS ?= --features internal
else
CARGO_BUILD_FLAGS ?= --features verified
endif

# RUST build.rs file will not understand (RAM_LEN)K notation
KB := 1024
ifneq (,$(filter %K,$(ROM_LEN)))
ROM_LEN_K = $(shell echo $(ROM_LEN) | sed 's/K//')
RIOTBOOT_FLASH_LEN = $(shell echo $$(( $(ROM_LEN_K) * $(KB) )) )
else
RIOTBOOT_FLASH_LEN = $(RIOTBOOT_LEN)
endif
ifneq (,$(filter %K,$(RAM_LEN)))
RAM_LEN_K = $(shell echo $(RAM_LEN) | sed 's/K//')
RIOTBOOT_RAM_LEN = $(shell echo $$(( $(RAM_LEN_K) * $(KB) )) )
else
RIOTBOOT_RAM_LEN = $(RAM_LEN)
endif

# export RIOT's RAM/ROM configuration to riotboot-rs build
RIOTBOOT_RAM_START    = $(RAM_START_ADDR)
RIOTBOOT_FLASH_START  = $(ROM_START_ADDR)
RIOTBOOT_SLOT0_OFFSET = $(SLOT0_OFFSET)
RIOTBOOT_SLOT1_OFFSET = $(SLOT1_OFFSET)

RIOTBOOT_RS_EXPORT_VARS ?= \
  RIOTBOOT_RAM_START \
  RIOTBOOT_RAM_LEN \
  RIOTBOOT_FLASH_START \
  RIOTBOOT_FLASH_LEN \
  RIOTBOOT_SLOT0_OFFSET \
  RIOTBOOT_SLOT1_OFFSET \
  #

$(foreach var,$(RIOTBOOT_RS_EXPORT_VARS),$(call target-export-variables,riotboot-rs,$(var)))

all:
	@$(Q)mkdir -p $(BINDIR)
	@$(Q) cd $(PKG_SOURCE_DIR) \
	      && rustup target add $(CARGO_TARGET) \
	      && cargo build --release --target $(CARGO_TARGET) $(CARGO_BUILD_FLAGS)
	@$(Q) cp $(PKG_SOURCE_DIR)/target/$(CARGO_TARGET)/release/riotboot-rs $(BINDIR)/riotboot-rs.elf
	@$(Q) $(OBJCOPY) -O binary $(BINDIR)/riotboot-rs.elf $(BINDIR)/riotboot-rs.bin

clean::
	rm -r $(PKG_SOURCE_DIR)/target/$(CARGO_TARGET)/release/
