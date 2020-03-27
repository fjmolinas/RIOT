APPLICATION = openwsn_test

BOARD ?= iotlab-m3

include ../Makefile.tests_common

BOARDSDIR = $(CURDIR)/external-boards

USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += openwsn_scheduler
USEMODULE += openwsn_ledpins

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
  USEMODULE += stdio_null
endif

USEMODULE += xtimer
USEMODULE += ipv6_addr
USEMODULE += ps
USEMODULE += od_string
USEMODULE += shell
USEMODULE += shell_commands

CFLAGS += -DDEVELHELP
CFLAGS += -DDEBUG_ASSERT_VERBOSE

include $(RIOTBASE)/Makefile.include
