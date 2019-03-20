#
SUIT_MODE ?= local

#
SUIT_VENDOR ?= RIOT
SUIT_VERSION ?= $(APP_VER)
SUIT_DEVICE_ID ?= $(BOARD)
SUIT_KEY ?= secret.key

#
SLOT0_SUIT_MANIFEST ?= $(BINDIR_APP)-slot0.riot.suit.$(APP_VER).bin
SLOT1_SUIT_MANIFEST ?= $(BINDIR_APP)-slot1.riot.suit.$(APP_VER).bin

ifeq (local,$(SUIT_MODE))
  $(info Using local suit update mode)
  include $(RIOTMAKE)/suit.local.inc.mk
else ifeq (http,$(SUIT_MODE))
  $(info Using HTTP suit update mode)
  include $(RIOTMAKE)/suit.http.inc.mk
else
  $(error unsupported suit mode '$(SUIT_MODE)')
endif

define manifest-recipe
  $(RIOTBASE)/dist/tools/suit_v1/gen_manifest.py \
	  --raw -k $(SUIT_KEY) \
	  -u $(SUIT_COAP_ROOT)/$(notdir $<) \
	  -d $(SUIT_DEVICE_ID) \
	  -V $(SUIT_VERSION) \
	  -e $(SUIT_VENDOR) \
	  --no-sign \
	  -o $@ \
	  $<
endef

$(SLOT0_SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SUIT_KEY)
	$(Q)$(manifest-recipe)

$(SLOT1_SUIT_MANIFEST): $(SLOT1_RIOT_BIN) $(SUIT_KEY)
	$(Q)$(manifest-recipe)

$(info $(SUIT_MANIFESTS))

suit/manifest: $(SUIT_MANIFESTS)

suit/genkey:
	$(RIOTBASE)/dist/tools/suit_v1/gen_key.py
