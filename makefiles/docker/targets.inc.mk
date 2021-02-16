# TODO
#        - first document only in advanced-build-system-tricks until mature
#        - We do not care for handling `clean` and other targets in parallel

# currently the BOARD is not always passed to docker, we always want to
DOCKER_ENVIRONMENT_CMDLINE += --env BOARD=$(BOARD)

################################################################################

# TODO: is it a good separator?
DOCKER_TARGET_SEPARATOR ?= __

# TODO:
#       - DOCKER_IMAGE should execute 32bit for native
#       - DOCKER_IMAGE needs native toolchain for flashing
#       - DOCKER_IMAGE general cleanup
#       - Should it be an extension of riot/riotbuild
#       - IPV6?
# HACK:
#       - currently riot/riotbuild is enough to test native samr21-xpro
# DOCKER_TARGET_IMAGE ?= riot/riotflash:latest
DOCKER_TARGET_IMAGE ?= $(DOCKER_IMAGE)

# TODO:
#       - This seems to be a ubuntuism, how is it handled in other dist?
DOCKER_TARGET_GROUPS ?= dialout plugdev

# TODO:
#       - Add PROG_DEV, document how to find it
#         PROG_DEV is needed for most BOARDS that do not have a BSL
#       - tap* should not be a PORT
DOCKER_TARGET_PORT_VARS ?= PORT PROG_DEV
DOCKER_TARGET_ENVIRONMENT += $(foreach var,$(DOCKER_TARGET_PORT_VARS),\
                                           --env $(var)=$(realpath $($(var))))

# TODO:
#       - Add SERIAL_TERM, SERIAL_PROG
#       - Migrate all SERIAL_* to SERIAL_TERM, SERIAL_PROG
DOCKER_TARGET_SERIAL_VARS ?= SERIAL DEBUG_ADAPTER_ID JLINK_SERIAL
DOCKER_TARGET_ENVIRONMENT += $(foreach var,$(DOCKER_TARGET_SERIAL_VARS),\
                                           --env $(var)=$($(var)))

# Export device variables for the target BOARD to docker
DOCKER_TARGET_FLAGS += $(DOCKER_TARGET_ENVIRONMENT)

# Need interactive for term
ifneq (,$(filter docker/%term docker/term% docker/test%,$(MAKECMDGOALS)))
  DOCKER_TARGET_FLAGS += --interactive
endif

# If all /dev need to be mapped
DOCKER_TARGET_MAP_ALL_DEV ?= 0
ifneq (0,$(DOCKER_TARGET_MAP_ALL_DEV))
  DOCKER_TARGET_VOLUMES ?= $(call docker_volume,/dev,/dev)
endif
DOCKER_TARGET_VOLUMES ?=
DOCKER_TARGET_FLAGS += $(DOCKER_TARGET_VOLUMES)

# --privileged can be added to succeed in any case without groups,
# /dev, device configuration
DOCKER_TARGET_PRIVILEGED ?= 0
ifneq (0,$(DOCKER_TARGET_PRIVILEGED))
  DOCKER_TARGET_FLAGS += --privileged
endif
DOCKER_TARGET_FLAGS += $(addprefix --group-add ,$(DOCKER_TARGET_GROUPS))
# Map listed devices
DOCKER_TARGET_FLAGS += $(foreach var,$(DOCKER_TARGET_PORT_VARS),\
                                     $(call map_existing_device,$($(var))))
map_existing_device = $(addprefix --device=,$(wildcard $1))

# Allow setting make args from command line like '-j'
DOCKER_TARGET_MAKE_ARGS ?=

.PHONY: docker/%

# Split multiple targets
DOCKER_TARGET_TARGETS = $(subst $(DOCKER_TARGET_SEPARATOR), ,$*)

docker/%:
	$(call docker_run_make,$(DOCKER_TARGET_TARGETS),$(DOCKER_TARGET_IMAGE),\
                           $(DOCKER_TARGET_FLAGS),$(DOCKER_TARGET_MAKE_ARGS))
