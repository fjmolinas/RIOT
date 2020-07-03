# name of the application
APPLICATION = driver_dw1000

# If no BOARD is found in the environment, use this default:
BOARD ?= dwm1001

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# required modules
USEMODULE += dw1000

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
