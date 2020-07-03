PKG_NAME=driver_dw1000
PKG_URL=https://github.com/bitcraze/libdw1000
PKG_VERSION=448a0efde42a258139cc8ca61fe53ff3bb4a9b94
PKG_LICENSE=Apache-2.0

PKG_SOURCE_LOCAL = /home/francisco/workspace/libdw1000

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-strict-prototypes
CFLAGS += -Wno-old-style-definition

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=driver_dw1000
