#
# Include this file if your Package needs to be checked out by git
#
PKG_DIR?=$(CURDIR)
PKG_BUILDDIR?=$(PKGDIRBASE)/$(PKG_NAME)
PKG_SOURCE_LOCAL ?= $(PKG_SOURCE_LOCAL_$(shell echo $(PKG_NAME) | tr a-z- A-Z_))

# allow overriding package source with local folder (useful during development)
ifneq (,$(PKG_SOURCE_LOCAL))
  include $(RIOTBASE)/pkg/local.mk
else

.PHONY: prepare git-download clean git-ensure-version

PKG_PATCHES = $(sort $(wildcard $(PKG_DIR)/patches/*.patch))

ifneq (,$(wildcard $(PKG_BUILDDIR)/.git-pkg))
PKG_HASH = $(shell cat $(PKG_BUILDDIR)/.git-pkg)
else
PKG_HASH = 0
endif

prepare: git-download

ifneq (,$(PKG_PATCHES))
git-download: $(PKG_BUILDDIR)/.git-patched
else
git-download: git-ensure-version
endif

GITFLAGS ?= -c user.email=buildsystem@riot -c user.name="RIOT buildsystem"
GITAMFLAGS ?= --no-gpg-sign --ignore-whitespace

ifneq (,$(wildcard $(PKG_BUILDDIR)/.git-patched))
PATCH_VERSION = $(shell cat $(PKG_BUILDDIR)/.git-patched)
else
PATCH_VERSION = 0
endif

ifneq (,$(PKG_PATCHES))
$(PKG_BUILDDIR)/.git-patched: git-ensure-version $(PKG_DIR)/Makefile $(PKG_DIR)/patches/*.patch
	if [ $(shell git -C $(PKG_BUILDDIR) rev-parse HEAD) != $(PATCH_VERSION) ] ; then \
		git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION) ; \
		git $(GITFLAGS) -C $(PKG_BUILDDIR) am $(GITAMFLAGS) "$(PKG_DIR)"/patches/*.patch ; \
		touch $@ ; \
		git -C $(PKG_BUILDDIR) rev-parse HEAD > $(PKG_BUILDDIR)/.git-patched ; \
	fi
endif

git-ensure-version: $(PKG_BUILDDIR)/.git-downloaded $(PKG_BUILDDIR)/.git-pkg
	if [ $(shell git -C $(PKG_BUILDDIR) rev-parse HEAD~$(words $(PKG_PATCHES))) != $(PKG_VERSION) ] ; then \
		git -C $(PKG_BUILDDIR) clean -xdff ; \
		git -C $(PKG_BUILDDIR) fetch "$(PKG_URL)" "$(PKG_VERSION)" ; \
		git -C $(PKG_BUILDDIR) checkout -f $(PKG_VERSION) ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
		touch $(PKG_BUILDDIR)/.git-pkg ; \
	fi

$(PKG_BUILDDIR)/.git-pkg:
	if [ $(shell git log --pretty="%H" -n 1 $(PKG_DIR)) != $(PKG_HASH) ] ; then \
		git -C $(PKG_BUILDDIR) clean -xdff ; \
		touch $(PKG_BUILDDIR)/.git-pkg ; \
		git log --pretty="%H" -n 1 $(PKG_DIR) > $(PKG_BUILDDIR)/.git-pkg ; \
	fi

$(PKG_BUILDDIR)/.git-downloaded:
		rm -Rf $(PKG_BUILDDIR) ; \
		mkdir -p $(PKG_BUILDDIR) ; \
		$(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_BUILDDIR)" ; \
		touch $@ ; \

clean::
	@test -d $(PKG_BUILDDIR) && { \
		rm $(PKG_BUILDDIR)/.git-patched ; \
		git -C $(PKG_BUILDDIR) clean -f ; \
		git -C $(PKG_BUILDDIR) checkout "$(PKG_VERSION)"; \
		make $(PKG_BUILDDIR)/.git-patched ; \
		touch $(PKG_BUILDDIR)/.git-downloaded ; \
	} > /dev/null 2>&1 || true

distclean::
	rm -rf "$(PKG_BUILDDIR)"

endif
