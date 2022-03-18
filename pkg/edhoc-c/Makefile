PKG_NAME = EDHOC-C
PKG_URL = https://github.com/future-proof-iot/EDHOC-C.git
PKG_VERSION = 1db007ac073d87029937252e1b3d160969628871
PKG_LICENSE = BSD-3-Clause

PKG_SOURCE_LOCAL = /home/francisco/workspace/EDHOC-C

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-newline-eof

.PHONY: edhoc-c_%

EDHOC_C_MODULES := $(filter edhoc-c_%,$(USEMODULE))

all: $(EDHOC_C_MODULES)
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=edhoc-c

edhoc-c_crypto_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/crypto -f $(RIOTBASE)/Makefile.base MODULE=$@ SRC=$*.c

edhoc-c_cbor_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/cbor -f $(RIOTBASE)/Makefile.base MODULE=$@ SRC=$*.c
