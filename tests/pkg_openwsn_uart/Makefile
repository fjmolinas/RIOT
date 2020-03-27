APPLICATION = openwsn_uart

BOARD ?= iotlab-m3

include ../Makefile.tests_common

USEPKG += openwsn
USEMODULE += openwsn_serial
USEMODULE += openwsn_ledpins
USEMODULE += openwsn_sctimer

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
  USEMODULE += stdio_null
endif

include $(RIOTBASE)/Makefile.include
