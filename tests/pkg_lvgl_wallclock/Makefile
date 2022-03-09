BOARD ?= feather-nrf52840
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEPKG += lvgl
USEMODULE += lvgl_contrib
USEMODULE += lvgl_contrib_touch

USEMODULE += lvgl_widget_slider
USEMODULE += lvgl_widget_switch
USEMODULE += lvgl_extra_widget_msgbox
USEMODULE += lvgl_extra_widget_tabview
USEMODULE += lvgl_extra_widget_menu
USEMODULE += lvgl_extra_layout_flex
USEMODULE += lvgl_extra_theme_default
USEMODULE += lvgl_extra_theme_default_dark

USEMODULE += event_timeout_ztimer
USEMODULE += event_thread
CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=THREAD_STACKSIZE_LARGE

# SDL requires more stack
ifeq (native,$(BOARD))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=64*1024
endif

CFLAGS += -DLV_MEM_SIZE=32*1024

EXTERNAL_MODULE_DIRS += $(CURDIR)/modules
USEMODULE += slotted_adv
USEMODULE += rtc_utils

# include nimble makefile tools
include $(RIOTBASE)/makefiles/pkg/nimble.adv.mk
$(eval $(call _add_ext_adv_instance,TIME_SERVER_ADV_INST))

include $(RIOTBASE)/Makefile.include

