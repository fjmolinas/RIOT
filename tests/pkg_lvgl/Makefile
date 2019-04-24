
BOARD ?= stm32f429i-disc

include ../Makefile.tests_common

#CFLAGS += -DST7735_128x128
#USEMODULE += st7735

USEMODULE += xtimer
USEPKG += lvgl

USEMODULE += ili9341
CFLAGS += -DILI9341_LE_MODE

CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

INCLUDES += -I$(CURDIR)/

include $(RIOTBASE)/Makefile.include