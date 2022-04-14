# name of your application
APPLICATION = suit_fs

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 0
CFLAGS += -LOG_LEVEL=LOG_NONE

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Networked File Server
CSP ?= 1
ifeq (1,$(CSP))
  USE_ETHOS ?= 0
  CFLAGS += -DCOAP_PORT=2
  USEPKG += libcsp
else
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_ipv6_router_default
  USEMODULE += gnrc_icmpv6_echo
  # Change this to 0 to not use ethos
  ifeq ($(BOARD),native)
    USE_ETHOS ?= 0
  else
    USE_ETHOS ?= 1
  endif
endif

# Add SUIT
USEMODULE += suit
USEMODULE += suit_transport_coap
# Configure one RAM regions with 2K
CFLAGS += -DCONFIG_SUIT_STORAGE_RAM_REGIONS=1
CFLAGS += -DCONFIG_SUIT_STORAGE_RAM_SIZE=2048
USEMODULE += suit_storage_ram
# Add mock application
F12R_BLOBS += $(CURDIR)/application/fletcher32_fc.bin
BLOBS += application/fletcher32_fc.bin
USEPKG += femto-container

ifeq (1,$(USE_ETHOS))
  USEMODULE += stdio_ethos
  USEMODULE += gnrc_uhcpc
  # ethos baudrate can be configured from make command
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  # make sure ethos and uhcpd are built
  TERMDEPS += host-tools
  # For local testing, run
  #
  #     $ cd dist/tools/ethos; sudo ./setup_network.sh riot0 2001:db8::0/64
  #
  #... in another shell and keep it running.
  export TAP ?= riot0
  TERMPROG = $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS = $(TAP) $(PORT)
endif

# Ensure both slot bin files are always generated and linked to avoid compiling
# during the test. This ensures that "BUILD_IN_DOCKER=1 make test"
# can rely on them being present without having to trigger re-compilation.
BUILD_FILES += $(SLOT_RIOT_ELFS:%.elf=%.bin)
# The test needs the linked slot binaries without header in order to be able to
# create final binaries with specific APP_VER values. The CI RasPi test workers
# don't compile themselves and re-create signed images, thus add the required
# files here so they will be submitted along with the test jobs.
TEST_EXTRA_FILES += $(SLOT_RIOT_ELFS) $(SUIT_SEC) $(SUIT_PUB)
# For now `examples/%/tests" still rely on the test applicaton being reset after
# a terminal is opened to synchronize.
TESTRUNNER_RESET_AFTER_TERM ?= 1

include $(CURDIR)/Makefile.suit.custom

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

suit/publish-fletcher: suit/publish-fs
	@true

suit/publish-fw: suit/publish-fs
	@true

include $(RIOTMAKE)/default-radio-settings.inc.mk
