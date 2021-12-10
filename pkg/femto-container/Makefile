PKG_NAME    = femto-container
PKG_URL     = https://github.com/future-proof-iot/Femto-Container
PKG_VERSION = 9c35b82dc1b7385ce3a40535baa3da61c3ede9c0
PKG_LICENSE = apache

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(PKG_DIR)/Makefile.femto MODULE=$(PKG_NAME)
