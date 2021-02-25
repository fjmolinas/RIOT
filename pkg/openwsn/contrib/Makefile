MODULE = openwsn

SRC := $(filter-out uart% sctimer_% radio_%,$(wildcard *.c))

ifneq (,$(filter openwsn_sctimer_rtt,$(USEMODULE)))
  SRC += sctimer_rtt.c
endif

ifneq (,$(filter openwsn_sctimer_ztimer,$(USEMODULE)))
  SRC += sctimer_ztimer.c
endif

ifneq (,$(filter openwsn_radio_hal,$(USEMODULE)))
  SRC += radio_hal.c
endif

ifneq (,$(filter openwsn_radio_netdev,$(USEMODULE)))
  SRC += radio_netdev.c
endif

ifneq (,$(filter openwsn_uart_cdc_acm,$(USEMODULE)))
  SRC += uart_cdc_acm.c
else
  SRC += uart.c
endif

include $(RIOTBASE)/Makefile.base
