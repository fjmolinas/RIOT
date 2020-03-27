/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the "board" bsp module
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "periph/pm.h"

#include "sctimer.h"
#include "board_ow.h"
#include "radio.h"

#include "debugpins_riot.h"
#include "ledpins_riot.h"
#include "uart_ow.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void board_init_openwsn(void)
{
    DEBUG("[board_ow]: init\n");

    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        DEBUG("[board_ow]: leds init\n");
        ledpins_riot_init(openwsn_ledpins_params);
    }

    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        DEBUG("[board_ow]: leds init\n");
        debugpins_riot_init(openwsn_debugpins_params);
    }

    DEBUG("[board_ow]: sctimer init\n");
    sctimer_init();

    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        DEBUG("[board_ow]: uart init\n");
        uart_init_openwsn();
    }
}

void board_sleep(void)
{
    /* no sleep support for now */
}

void board_reset(void)
{
    DEBUG("[board_ow]: reset\n");
    pm_reboot();
}
