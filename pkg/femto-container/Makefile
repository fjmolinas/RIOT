PKG_NAME    = femto-container
PKG_URL     = https://github.com/future-proof-iot/Femto-Container
PKG_VERSION = 6987a20c1b3e3e5721a15a102e40d941c6bd1d81
PKG_LICENSE = apache

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(PKG_DIR)/Makefile.femto MODULE=$(PKG_NAME)
