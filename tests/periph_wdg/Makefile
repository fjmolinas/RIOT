include ../Makefile.tests_common
BOARD ?= nucleo-l152re

TEST_ON_CI_WHITELIST += all

FEATURES_REQUIRED += periph_wdg

USEMODULE += shell
USEMODULE += shell_commands

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
