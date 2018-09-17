include ../Makefile.tests_common

BOARD := stm32f429i-disc1

DEVELHELP ?= 1

USEMODULE += ili9341
USEMODULE += xtimer

CFLAGS += -DILI9341_LE_MODE

include $(RIOTBASE)/Makefile.include
