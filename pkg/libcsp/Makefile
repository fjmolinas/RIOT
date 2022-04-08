PKG_NAME=libcsp
PKG_URL=https://github.com/libcsp/libcsp.git
PKG_VERSION=038c2082061bba449a4f2a5cd7ff31ed435f4cab
PKG_LICENSE=MIT

CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-format

IGNORE_MODULES := libcsp_contrib \
                  libcsp_drivers_can \
                  libcsp_shell \
                  libcsp_shell_server \
                  #

LIBCSP_MODULES := $(filter-out $(IGNORE_MODULES),$(filter libcsp_%,$(USEMODULE)))

include $(RIOTBASE)/pkg/pkg.mk

all: $(LIBCSP_MODULES)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/$(PKG_NAME).mk

libcsp_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/interfaces -f $(CURDIR)/$(PKG_NAME)_$*.mk
