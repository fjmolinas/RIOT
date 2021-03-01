/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 *
 * @file
 * @brief       RIOT opentimer types variable declaration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#ifndef OPENTIMERS_TYPES_H
#define OPENTIMERS_TYPES_H

#include "opendefs.h"

#include "ztimer/periodic.h"
#include "memarray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*opentimers_cbt)(void* arg);

typedef struct {
    ztimer_periodic_t timer; /**< time object */
    opentimers_cbt cb;       /**< callback */
    bool isr;                /**< whether the timer is executed in ISR context */
    time_type_t unit;        /**< time unit */
    timer_type_t type;       /**< the timer type */
    uint8_t prio;            /**< when opentimer push a task, use timer_task_prio
                                  to mark the priority */
} opentimers_t;

typedef opentimers_t * opentimers_id_t;

/**
 * @brief   OpenWSN opentimer variables structure
 */
typedef struct {
    opentimers_t timer_buff[MAX_NUM_TIMERS]; /**< Timer buffer */
    memarray_t memarray;                     /**< Memarray management */
    PORT_TIMER_WIDTH current_compare_value;   /**< currentCompareValue */
} opentimers_vars_t;

#ifdef __cplusplus
}
#endif

#endif /* OPENTIMERS_TYPES_H */
