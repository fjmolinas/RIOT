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

USEMODULE += event_timeout
USEMODULE += event_thread
CFLAGS += -DEVENT_THREAD_MEDIUM_STACKSIZE=THREAD_STACKSIZE_LARGE

# SDL requires more stack
ifeq (native,$(BOARD))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=64*1024
endif

CFLAGS += -DLV_MEM_SIZE=32*1024

include $(RIOTBASE)/Makefile.include
