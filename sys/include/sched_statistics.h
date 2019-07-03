/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_sched_statistics Scheduler statistics
 * @ingroup     core_sched
 * @brief       When including this module scheduler statistics
 *              (@ref schedstat_t) for a thread will be updated on every
 *              @ref sched_run().
 *
 * @note        If auto_init is disabled `auto_init_sched_statistics()` needs to be
 *              called.
 *
 * @{
 *
 * @file
 * @brief       Scheduler statistics definitons
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef SCHED_STATISTICS_H
#define SCHED_STATISTICS_H

#include "kernel_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 *  @brief Scheduler statistics
 */
typedef struct {
    uint32_t laststart;      /**< Time stamp of the last time this thread was
                                  scheduled to run */
    unsigned int schedules;  /**< How often the thread was scheduled to run */
    uint64_t runtime_ticks;  /**< The total runtime of this thread in ticks */
} schedstat_t;

/**
 *  @brief Expose thread statistics table
 */
extern schedstat_t sched_pidlist[KERNEL_PID_LAST + 1];

#ifdef __cplusplus
}
#endif

#endif /* SCHED_STATISTICS_H */
/** @} */
