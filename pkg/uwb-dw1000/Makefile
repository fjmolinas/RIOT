PKG_NAME=uwb-dw1000
PKG_URL=https://github.com/fjmolinas/uwb-dw1000/
PKG_VERSION=715f6687e3b2b77f63303e3c231fbeb2c8588e97
PKG_LICENSE=Apache-2.0

# PKG_SOURCE_LOCAL = /home/francisco/workspace/decawave/uwb-dw1000

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-missing-declarations
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-return-type
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-variable
CFLAGS += -fms-extensions

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/hw/drivers/uwb/uwb_dw1000/src -f $(RIOTPKG)/uwb-dw1000/uwb-dw1000.mk MODULE=uwb-dw1000
