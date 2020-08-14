/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 * @{
 *
 * @file
 * @brief       Timer abstraction layer RIOT adaption
 *
 */

#ifndef DPL_HAL_TIMER
#define DPL_HAL_TIMER

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* HAL timer callback */
typedef xtimer_callback_t hal_timer_cb;

/* The HAL timer structure. */
struct hal_timer
{
    xtimer_t timer;
};

#ifdef __cplusplus
}
#endif

#endif /* DPL_HAL_TIMER*/
