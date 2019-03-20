# 
SUIT_MAKEFILE ?= local

#
SUIT_MANIFEST ?= $(BINDIR_APP)-riot.suitv4.$(APP_VER).bin
SUIT_MANIFEST_SIGNED ?= $(BINDIR_APP)-riot.suitv4_signed.$(APP_VER).bin

# Long manifest names require more buffer space when parsing
export CFLAGS += -DSOCK_URLPATH_MAXLEN=128

SUIT_VENDOR ?= "riot-os.org"
SUIT_SEQNR ?= $(APP_VER)
SUIT_DEVICE_ID ?= $(BOARD)

#
# SUIT encryption keys
#

# Specify key to use.
# Will use $(SUIT_KEY_DIR)/$(SUIT_KEY) $(SUIT_KEY_DIR)/$(SUIT_KEY).pub as
# private/public key files, similar to how ssh names its key files.
SUIT_KEY ?= default

ifeq (1, $(RIOT_CI_BUILD))
  SUIT_KEY_DIR ?= $(BINDIR)
else
  SUIT_KEY_DIR ?= $(RIOTBASE)/keys
endif

SUIT_SEC ?= $(SUIT_KEY_DIR)/$(SUIT_KEY)
SUIT_PUB ?= $(SUIT_KEY_DIR)/$(SUIT_KEY).pub

SUIT_PUB_HDR = $(BINDIR)/riotbuild/public_key.h
SUIT_PUB_HDR_DIR = $(dir $(SUIT_PUB_HDR))
CFLAGS += -I$(SUIT_PUB_HDR_DIR)
BUILDDEPS += $(SUIT_PUB_HDR)

$(SUIT_SEC) $(SUIT_PUB): | $(CLEAN)
	@echo suit: generating key pair in $(SUIT_KEY_DIR)
	@mkdir -p $(SUIT_KEY_DIR)
	@$(RIOTBASE)/dist/tools/suit_v4/gen_key.py $(SUIT_SEC) $(SUIT_PUB)

# set FORCE so switching between keys using "SUIT_KEY=foo make ..."
# triggers a rebuild even if the new key would otherwise not (because the other
# key's mtime is too far back).
$(SUIT_PUB_HDR): $(SUIT_PUB) FORCE | $(CLEAN)
	@mkdir -p $(SUIT_PUB_HDR_DIR)
	@cp $(SUIT_PUB) $(SUIT_PUB_HDR_DIR)/public.key
	@cd $(SUIT_PUB_HDR_DIR) && xxd -i public.key \
	  | '$(LAZYSPONGE)' $(LAZYSPONGE_FLAGS) '$@'

suit/genkey: $(SUIT_SEC) $(SUIT_PUB)

#
$(SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	$(RIOTBASE)/dist/tools/suit_v4/gen_manifest.py \
	  --template $(RIOTBASE)/dist/tools/suit_v4/test-2img.json \
	  --urlroot $(SUIT_COAP_ROOT) \
	  --seqnr $(SUIT_SEQNR) \
	  --uuid-vendor $(SUIT_VENDOR) \
	  --uuid-class $(SUIT_DEVICE_ID) \
	  --offsets $(SLOT0_OFFSET),$(SLOT1_OFFSET) \
	  -o $@ \
	  $^

$(SUIT_MANIFEST_SIGNED): $(SUIT_MANIFEST)
	$(RIOTBASE)/dist/tools/suit_v4/sign-04.py \
	  $(SUIT_SEC) $(SUIT_PUB) $< $@

ifeq (local,$(SUIT_MAKEFILE))
  include $(RIOTMAKE)/suit.v4.local.inc.mk
else
  include $(SUIT_MAKEFILE)
endif