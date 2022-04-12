# name of your application
APPLICATION = gcoap_fs

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
USEMODULE += coapfileserver
ifeq (1,$(CSP))
  CFLAGS += -DCOAP_PORT=2
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
CFLAGS += -DFATFS_FFCONF_OPT_USE_LFN=1

# Enable test_utils_interactive_sync, only used when running automatic test
DEFAULT_MODULE += test_utils_interactive_sync

# Change this to 0 to not use ethos
USE_ETHOS ?= 0

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

# FATFS for the fileserver
FATFS_IMAGE_FILE_SIZE_MIB ?= 128
FATFS_IMAGE_FILE ?= $(RIOTBASE)/examples/gcoap_fs/riot_fatfs_disk.img
ifeq ($(BOARD),native)
  CFLAGS += -DMTD_NATIVE_FILENAME=\"$(FATFS_IMAGE_FILE)\"
  CFLAGS += -DMTD_SECTOR_NUM=\($(FATFS_IMAGE_FILE_SIZE_MIB)*1024*1024/MTD_SECTOR_SIZE\)
endif

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

fatfs-image:
	$(Q)if [ ! -f "$(FATFS_IMAGE_FILE)" ]; then \
  echo "Creating $(FATFS_IMAGE_FILE) ..."; \
  dd if=/dev/zero of=$(FATFS_IMAGE_FILE) bs=1M count=$(FATFS_IMAGE_FILE_SIZE_MIB); \
  mkfs.fat $(FATFS_IMAGE_FILE); \
  fi

host-tools:
	$(Q)env -u CC -u CFLAGS $(MAKE) -C $(RIOTTOOLS)

include $(RIOTMAKE)/default-radio-settings.inc.mk
