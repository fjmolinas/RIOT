include ../Makefile.tests_common

# Use CSP by default
CSP ?= 1

ifeq (1,$(CSP))
  CFLAGS += -DCOAP_PORT=2
  USEPKG += libcsp
  FEATURES_REQUIRED += periph_can
else
  # Include packages that pull up and auto-init the link layer.
  # NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_ipv6_default
  USEMODULE += netdev_default
endif

USEMODULE += sock_udp
USEMODULE += nanocoap_sock

# Required by test
USEMODULE += od
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
