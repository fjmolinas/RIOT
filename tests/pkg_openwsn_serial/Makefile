APPLICATION = openwsn_serial

BOARD ?= iotlab-m3

include ../Makefile.tests_common

USEPKG += openwsn
USEMODULE += openwsn_serial
USEMODULE += openwsn_sctimer
USEMODULE += openwsn_ledpins

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
  USEMODULE += stdio_null
endif

CFLAGS += -Wno-unused-parameter

CFLAGS += -DOPENSERIAL_PRINTF

include $(RIOTBASE)/Makefile.include