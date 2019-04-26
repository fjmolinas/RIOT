#!/bin/sh
#
# Flash configuration field check script for Freescale Kinetis MCUs.
#
# This script is supposed to be called from RIOTs
# unified OpenOCD script (dist/tools/openocd/openocd.sh).
#
# Syntax: check-fcfield-hex.sh $HEXFILE
#
# @author       Jonas Remmert <j.remmert@phytec.de>
# @author       Johann Fischer <j.fischer@phytec.de>
# @author       Joakim Nohlgård <joakim.nohlgard@eistec.se>
# @author       Francisco Molina <francisco.molina@inria.fr>

if [ $# -ne 1 ]; then
    echo "Usage: $0 BINFILE"
    echo "Checks the flash configuration field protection bits to avoid flashing a locked image to a Kinetis MCU (protection against accidental bricking)."
    exit 2
fi

if [ -n "${IMAGE_OFFSET}" ]; then
    FCFIELD=$((${RIOTBOOT_HDR_LEN}+${VTOR_OFFSET}))
    FCFIELD_START=`printf "0x%08x\n" $FCFIELD`
    FCFIELD_END=`printf "0x%08x\n" $((${FCFIELD}+${FCFIELD_OFFSET}))`
    FCFIELD_AWK_REGEX=`printf "^ %04x\n" $((0x400+${RIOTBOOT_HDR_LEN}))`
else
    FCFIELD_START='0x400'
    FCFIELD_END='0x410'
    FCFIELD_AWK_REGEX='^ 0400 '
fi

BINFILE="$1"
ACTUAL_FCFIELD=$(arm-none-eabi-objdump --start-address=${FCFIELD_START} --stop-address=${FCFIELD_END} -bbinary -marm ${BINFILE} -s | awk -F' ' "/${FCFIELD_AWK_REGEX}/ { print \$2 \$3 \$4 \$5; }")
# Allow any FOPT flags configuration (".." in the pattern)
EXPECTED_FCFIELD="^fffffffffffffffffffffffffe..ffff$"

if ! printf '%s' "${ACTUAL_FCFIELD}" | grep -q "${EXPECTED_FCFIELD}"; then
    echo "Danger of bricking the device during flash!"
    echo "Flash configuration field of ${BINFILE}:"
    arm-none-eabi-objdump --start-address=${FCFIELD_START} --stop-address=${FCFIELD_END} ${BINFILE} -s
    echo "Abort flash procedure!"
    exit 1
fi
echo "${BINFILE} is not locked."
exit 0
