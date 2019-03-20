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
  include $(RIOTMAKE)/suit.local.inc.mk
else ifeq (http,$(SUIT_MODE))
  include $(RIOTMAKE)/suit.http.inc.mk
else
  $(error unsupported suit mode '$(SUIT_MODE)')
endif

suit/genkey:
	$(RIOTBASE)/dist/tools/suit_v1/gen_key.py
