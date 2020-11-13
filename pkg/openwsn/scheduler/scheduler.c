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
 * @file
 * @brief       Event based implementation of OpenWSN scheduler
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */
#include "event.h"
#include "event/callback.h"
#include "memarray.h"
#include "periph/pm.h"

#include "scheduler.h"
#include "debugpins.h"
#include "leds.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_ERROR
#endif
#include "log.h"

static scheduler_vars_t _scheduler_vars;
/* event queues for every priority */
static event_queue_t _queues[TASKPRIO_MAX];

#if SCHEDULER_DEBUG_ENABLE
scheduler_dbg_t scheduler_dbg;
#endif

void scheduler_init(void)
{
#if SCHEDULER_DEBUG_ENABLE
    memset(&scheduler_dbg, 0, sizeof(scheduler_dbg_t));
#endif
    memset(&_scheduler_vars, 0, sizeof(scheduler_vars_t));
    memarray_init(&_scheduler_vars.memarray, &_scheduler_vars.task_buff,
                  sizeof(event_callback_t), TASK_LIST_DEPTH);

    event_queues_init_detached(_queues, TASKPRIO_MAX);
}

void scheduler_start(unsigned state)
{
    irq_restore(state);

    /* claim all queues */
    event_queues_claim(_queues, TASKPRIO_MAX);

    /* wait for events */
    event_t *event;
#ifdef MODULE_PM_LAYERED
#if IS_USED(MODULE_CC2538_RF)
    /* unblock modes that have RAM retention */
    pm_unblock(PM_NUM_MODES - 2);
#endif
    pm_unblock(PM_NUM_MODES - 1);
#endif
    while ((event = event_wait_multi(_queues, TASKPRIO_MAX))) {
#ifdef MODULE_PM_LAYERED
        pm_block(PM_NUM_MODES - 1);
#endif
        debugpins_task_set();
        event->handler(event);
        /* remove from task list */
        memarray_free(&_scheduler_vars.memarray, event);
#if SCHEDULER_DEBUG_ENABLE
        scheduler_dbg.numTasksCur--;
#endif
        debugpins_task_clr();
#ifdef MODULE_PM_LAYERED
        pm_unblock(PM_NUM_MODES - 1);
#endif
    }
}

void scheduler_push_task(task_cbt cb, task_prio_t prio)
{
    unsigned state = irq_disable();
    /* get a free event from the queue */
    event_callback_t *event = memarray_calloc(&_scheduler_vars.memarray);

    if (!event) {
        /* task list has overflown. This should never happen! */
        LOG_ERROR("[openos/scheduler]: critical, task list overflow\n");
        leds_error_blink();
        pm_off();
    }

    event_callback_init(event, (void (*)(void *)) cb, NULL);
    event_post(&_queues[prio], (event_t *)event);

#if SCHEDULER_DEBUG_ENABLE
    scheduler_dbg.numTasksCur++;
    if (scheduler_dbg.numTasksCur > scheduler_dbg.numTasksMax) {
        scheduler_dbg.numTasksMax = scheduler_dbg.numTasksCur;
    }
    scheduler_dbg.numTasksSum += scheduler_dbg.numTasksCur;
#endif

    irq_restore(state);
}

#if SCHEDULER_DEBUG_ENABLE
uint8_t scheduler_debug_get_TasksCur(void)
{
   return scheduler_dbg.numTasksCur;
}

uint32_t scheduler_debug_get_TasksSum(void)
{
   return scheduler_dbg.numTasksSum;
}

void scheduler_debug_TasksSum_reset(void)
{
    INTERRUPT_DECLARATION();

    DISABLE_INTERRUPTS();

    scheduler_dbg.numTasksSum = 0;
    ENABLE_INTERRUPTS();
}

uint8_t scheduler_debug_get_TasksMax(void)
{
   return scheduler_dbg.numTasksMax;
}
#endif
