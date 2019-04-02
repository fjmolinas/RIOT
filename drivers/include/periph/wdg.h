/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_wdg WDG
 * @ingroup     drivers_periph
 * @brief       watchdog timer peripheral driver
 *
 * @{
 *
 * @file        wdg.h
 * @brief       watchdog peripheral interface definitions
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef PERIPH_WDG_H
#define PERIPH_WDG_H

#include <stdint.h>
#include "timex.h"

/**
 * @brief    WDG return values
 */
enum {
    WDG_OK      =  0,
    WDG_ERROR   = -1,
};

/**
 * @brief    Start watchdog timer
 */
void wdg_start(void);

/**
 * @brief    Stop watchdog timer
 */
void wdg_stop(void);

/**
 * @brief    Reset the watchdog timer counter, delay system reset
 */
void wdg_kick(void);

/**
 * @brief    Set up the wdg timer, only use max_time if normal operation
 *           set min_time and max_time for windowed timer.
 *
 * @param[in] min_time       lower bound for windowed watchdog
 * @param[in] max_time       upper bound for windowed watchdog, time before
 *                           reset for normal watchdog
 *
 * @return                  -1 WDG_ERROR on configuration error
 * @return                   0 WDG_OK when configured correctly
 */
int wdg_setup(uint32_t min_time, uint32_t max_time);

#endif /* PERIPH_WDG_H */
