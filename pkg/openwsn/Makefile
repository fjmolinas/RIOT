PKG_NAME=openwsn-fw
PKG_URL=https://github.com/openwsn-berkeley/openwsn-fw.git
	PKG_VERSION=71bb5d74f4e4e4fd82c31dd0a86281c0c067986b
	PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)
	PKG_LICENSE=BSD-3-Clause

.PHONY: all

OPENWSN_PATH := $(HOME)/Workspace/openwsn/openwsn-fw

all:
	rm -rf $(PKG_BUILDDIR) && mkdir -p $(PKG_BUILDDIR)
	rsync -a $(OPENWSN_PATH) $(dir $(PKG_BUILDDIR)) --exclude .git
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
