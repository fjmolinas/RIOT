PKG_NAME=lvgl
PKG_URL=https://github.com/littlevgl/lvgl
PKG_VERSION=ef0f8b29cf0da7313d2b355817edb5b23587767b
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/lv_core -f $(CURDIR)/Makefile.lv_core
	"$(MAKE)" -C $(PKG_BUILDDIR)/lv_draw -f $(CURDIR)/Makefile.lv_draw
	"$(MAKE)" -C $(PKG_BUILDDIR)/lv_fonts -f $(CURDIR)/Makefile.lv_fonts
	"$(MAKE)" -C $(PKG_BUILDDIR)/lv_hal -f $(CURDIR)/Makefile.lv_hal
	"$(MAKE)" -C $(PKG_BUILDDIR)/lv_misc -f $(CURDIR)/Makefile.lv_misc
	"$(MAKE)" -C $(PKG_BUILDDIR)/lv_objx -f $(CURDIR)/Makefile.lv_objx
	"$(MAKE)" -C $(PKG_BUILDDIR)/lv_themes -f $(CURDIR)/Makefile.lv_themes

include $(RIOTBASE)/pkg/pkg.mk
