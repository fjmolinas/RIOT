APPLICATION = openwsn_debugpins

BOARD ?= iotlab-m3

include ../Makefile.tests_common

USEPKG += openwsn

USEMODULE += xtimer
USEMODULE += openwsn_debugpins

CFLAGS +=-DAUTO_INIT_LED0

include $(RIOTBASE)/Makefile.include
