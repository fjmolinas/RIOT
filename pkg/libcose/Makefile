PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=8b5f651c3203682a2d98121cd3e5c844cb2b4c36
PKG_LICENSE=LGPL

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/crypt -f $(CURDIR)/Makefile.$(PKG_NAME)_crypt
