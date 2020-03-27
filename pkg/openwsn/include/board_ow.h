/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * RIOT HAL is provided as a new "board", a "RIOT board" to OpenWSN hardware
 * abstraction interfaces.
 *
 * @file
 * @brief       RIOT adaption-specific definition of the "uart" bsp module.
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 *
 * @}
 */

#ifndef __BOARD_OW_H
#define __BOARD_OW_H

#include "board_info.h"

/**
 * @brief   OpenWSN `board.h` enum definitions
 */
typedef enum {
    DO_NOT_KICK_SCHEDULER,
    KICK_SCHEDULER,
} kick_scheduler_t;


/**
 * @brief   Initialize OpenWSN bsp board adaptation
 *
 * Most initialization functions are not needed since there are initialized by
 * RIOT's `auto_init` module. Only OpenWSN specfic interfaces will be initialzed
 * here: `sctimer`, `openwsn_ledpins`, `openwsn_debugpins` & `uart_ow`.
 *
 */
void board_init_openwsn(void);

/**
 * @brief   Resets the board
 *
 */
void board_reset(void);

/**
 * @brief   Function definitions that we do not use in RIOT but that need to be
 *          defined for OpenWSN. Sleep is handled by `pm_layered` if enabled for
 *          the platform.
 */
void board_sleep(void);


#endif
