APPLICATION = twr-aloha

# If no BOARD is found in the environment, use this default:
BOARD ?= dwm1001

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Include uwb-core, uwb-dw1000 and ranging api
USEPKG += uwb-core
USEPKG += uwb-dw1000
USEMODULE += uwb-core_rng

# Include all ranging algorithms
USEMODULE += uwb-core_twr_ss
USEMODULE += uwb-core_twr_ss_ack
USEMODULE += uwb-core_twr_ss_ext
USEMODULE += uwb-core_twr_ds
USEMODULE += uwb-core_twr_ds_ext

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Set the device role 0x0 for tag 0x4 for an anchor
DW1000_ROLE ?= 0x00
CFLAGS += -DDW1000_ROLE_DEFAULT=$(DW1000_ROLE)

# All uwb-core applications need to enable `-fms-extensions`
CFLAGS += -fms-extensions
# Enable verbose mode to get all logs
CFLAGS += -DMYNEWT_VAL_RNG_VERBOSE=2
# Enable RX diagnostics
CFLAGS += -DDW1000_RX_DIAGNOSTIC=1

# Fix the TWR alogorithm:
# - UWB_DATA_CODE_SS_TWR
# - UWB_DATA_CODE_SS_TWR_EXT
# - UWB_DATA_CODE_SS_TWR_ACK
# - UWB_DATA_CODE_DS_TWR
# - UWB_DATA_CODE_DS_TWR_EXT
UWB_FIXED_TWR_ALGORITHM ?= UWB_DATA_CODE_SS_TWR
# Uncomment to fix the TWR algoritm
# CFLAGS += -DUWB_FIXED_TWR_ALGORITHM=$(UWB_FIXED_TWR_ALGORITHM)

# Enable for debugging
CFLAGS += -DDEBUG_ASSERT_VERBOSE

include $(RIOTBASE)/Makefile.include
