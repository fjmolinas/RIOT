/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sched_statistics
 * @{
 *
 * @file
 * @brief       Scheduler statistics implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "sched.h"
#include "xtimer.h"
#include "sched_statistics.h"

schedstat_t sched_pidlist[KERNEL_PID_LAST + 1];

void sched_statistics_cb(uint32_t active_thread, uint32_t next_thread) {

    uint32_t now = xtimer_now().ticks32;

    if((thread_getstatus(active_thread) != STATUS_STOPPED) && \
       (thread_getstatus(active_thread) != STATUS_NOT_FOUND)) {
        schedstat_t *active_stat = &sched_pidlist[active_thread];
        if (active_stat->laststart) {
            active_stat->runtime_ticks += now - active_stat->laststart;
        }
    }

    schedstat_t *next_stat = &sched_pidlist[next_thread];
    next_stat->laststart = now;
    next_stat->schedules++;
}

void auto_init_sched_statistics(void) {
    sched_register_cb(sched_statistics_cb);
}
