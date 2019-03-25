/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_wdg
 * @ingroup     drivers_periph
 * @brief       Common watchdog interface
 *
 * @{
 *
 * @file		wdg.h
 * @brief       watchdog peripheral interface
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef PERIPH_WDG_H_
#define PERIPH_WDG_H_

#include <stdint.h>
#include "timex.h"

/**
 * @brief   Possible WDG return values
 */
enum {
    WDG_OK         =  0,
    WDG_ERROR      = -1,
};

/**
 * @brief    Enable watchdog timer
 */
void wdg_enable(void);

/**
 * @brief    Disable watchdog timer
 */
void wdg_disable(void);

/**
 * @brief    Reset the watchdog timer
 */
void wdg_reset(void);

/**
 * @brief    Reset the watchdog timer
 */
uint32_t wdg_max_timeout(void);

/**
 * @brief    Reset the watchdog timer
 */
uint32_t wdg_min_timeout(void);


/**
 * @brief    Sets the time before a wdg reset, best effort approximate value
 *
 * @param[out] time_set     actual time in us for wdg reset , time_set~time
 *
 * @return                  0 When out of bounds
 * @return                  uint32_t with the set reset time
 */
int wdg_init(uint32_t rst_time);

#endif /* PERIPH_WDG_H_ */
