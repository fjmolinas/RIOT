# name of your application
APPLICATION = suit_fs

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Networked File Server
CSP ?= 1
USEMODULE += gcoap
ifeq (1,$(CSP))
  CFLAGS += -DCOAP_PORT=2
  CFLAGS += -DGOAP_PORT=2
  CFLAGS += -DCONFIG_GCOAP_PORT=2
  USEPKG += libcsp
else
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_ipv6_router_default
  USEMODULE += gnrc_icmpv6_echo
endif

# Use VFS
USEMODULE += vfs
USEMODULE += vfs_default
USEMODULE += vfs_auto_format
CFLAGS=-DFATFS_FFCONF_OPT_USE_LFN=1

# Add SUIT
USEMODULE += suit
USEMODULE += suit_transport_coap
USEMODULE += suit_storage_vfs
# Display a progress bar during firmware download
USEMODULE += progress_bar

# Enable test_utils_interactive_sync, only used when running automatic test
DEFAULT_MODULE += test_utils_interactive_sync

ifeq ($(BOARD),native)
  USE_ETHOS ?= 0
  # Configure two RAM regions with 2K each
  CFLAGS += -DCONFIG_SUIT_STORAGE_RAM_REGIONS=2
  CFLAGS += -DCONFIG_SUIT_STORAGE_RAM_SIZE=2048
endif

# Change this to 0 to not use ethos
USE_ETHOS ?= 1

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

include $(CURDIR)/Makefile.suit.native

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

include $(RIOTMAKE)/default-radio-settings.inc.mk
