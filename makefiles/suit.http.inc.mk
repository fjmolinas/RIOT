# Publish ID can be whatever the user prefer. The OTA server will take care of
# it.
SUIT_PUBLISH_ID ?= $(BOARD)_$(APPLICATION)

# Some requirements to publish and notify updates to the server
SUIT_OTA_SERVER_URL ?= http://localhost:8080
SUIT_OTA_SERVER_PUBLISH_EP ?= publish
SUIT_OTA_SERVER_NOTIFY_EP ?= notify
SUIT_OTA_SERVER_COAP_URL_EP ?= coap/url

# Only 2 manifest files are required for the build system
SUIT_MANIFESTS := $(SLOT0_SUIT_MANIFEST) \
                  $(SLOT1_SUIT_MANIFEST)

# The OTA server knows where the device can fetch the slots and manifest
SUIT_COAP_ROOT := $(shell curl -X GET \
      $(SUIT_OTA_SERVER_URL)/$(SUIT_OTA_SERVER_COAP_URL_EP)/$(SUIT_PUBLISH_ID))

define manifest-recipe
  $(info Manifest contains CoAP root path: $(SUIT_COAP_ROOT))
  $(RIOTBASE)/dist/tools/suit_v1/gen_manifest.py \
	  --raw -k $(SUIT_KEY) \
	  -u $(SUIT_COAP_ROOT)/$(notdir $<) \
	  -d $(SUIT_DEVICE_ID) \
	  -V $(SUIT_VERSION) \
	  -e $(SUIT_VENDOR) \
	  --no-sign \
	  -o $@ \
	  $< > /dev/null
endef

$(SLOT0_SUIT_MANIFEST): $(SLOT0_RIOT_BIN) $(SUIT_KEY)
	@$(manifest-recipe)

$(SLOT1_SUIT_MANIFEST): $(SLOT1_RIOT_BIN) $(SUIT_KEY)
	@$(manifest-recipe)

suit/manifest: $(SUIT_MANIFESTS)

OTA_CLIENT_CMD = python3 $(RIOTBASE)/dist/tools/suit_v1/otaclient.py

suit/publish: $(SUIT_MANIFESTS) $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	$(info ##### $^)
	$(Q)$(OTA_CLIENT_CMD) --publish-id $(SUIT_PUBLISH_ID) --files $^

suit/notify:
	$(Q)$(OTA_CLIENT_CMD) --publish-id $(SUIT_PUBLISH_ID) --notify $(SUIT_CLIENT)
