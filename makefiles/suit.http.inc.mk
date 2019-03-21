# Publish ID can be whatever the user prefer. The OTA server will take care of
# it.
SUIT_PUBLISH_ID ?= $(BOARD)_$(APPLICATION)

# Required for manifest generation
SUIT_COAP_SERVER ?= [::1]
SUIT_COAP_ROOT ?= coap://$(SUIT_COAP_SERVER)/$(SUIT_PUBLISH_ID)

# Some requirements to publish and notify updates to the server
SUIT_OTA_SERVER_URL ?= http://localhost:8080
SUIT_OTA_SERVER_PUBLISH_EP ?= publish
SUIT_OTA_SERVER_NOTIFY_EP ?= notify

# Only 2 manifest files are required for the build system
SUIT_MANIFESTS := $(SLOT0_SUIT_MANIFEST) \
                  $(SLOT1_SUIT_MANIFEST)

OTA_CLIENT_CMD = python3 $(RIOTBASE)/dist/tools/suit_v1/otaclient.py

suit/publish: $(SUIT_MANIFESTS) $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)
	$(Q)$(OTA_CLIENT_CMD) --publish-id $(SUIT_PUBLISH_ID) --files $^

suit/notify:
	$(Q)$(OTA_CLIENT_CMD) --publish-id $(SUIT_PUBLISH_ID) --notify $(SUIT_CLIENT)
