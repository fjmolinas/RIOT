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

#include "board.h"
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

#ifdef MODULE_PM_LAYERED
/* keep track if the lowest allowed mode is already blocked */
static bool _blocked = false;
#endif

static void _pm_block(void)
{
#ifdef MODULE_PM_LAYERED
    if (!_blocked) {
        _blocked = true;
        pm_block(OPENWSN_SCHEDULER_PM_BLOCKER);
    }
#endif
}

static void _pm_unblock(void)
{
#ifdef MODULE_PM_LAYERED
    if (_blocked) {
        _blocked = false;
        pm_unblock(OPENWSN_SCHEDULER_PM_BLOCKER);
    }
#endif
}

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

/* verbatim copy of event_wait_multi() but additionally calling pm_unblock()
   when queues are empty */
event_t *_event_wait_multi(event_queue_t *queues, size_t n_queues)
{
    assert(queues && n_queues);
    event_t *result;

    do {
        unsigned state = irq_disable();
        for (size_t i = 0; i < n_queues; i++) {
            result = container_of(clist_lpop(&queues[i].event_list),
                                  event_t, list_node);
            if (result) {
                break;
            }
        }
        irq_restore(state);
        if (result == NULL) {
            if (IS_USED(MODULE_PM_LAYERED)) {
                _pm_unblock();
            }
            thread_flags_wait_any(THREAD_FLAG_EVENT);
        }
    } while (result == NULL);

    result->list_node.next = NULL;
    return result;
}

void scheduler_start(unsigned state)
{
    irq_restore(state);

    /* claim all queues */
    event_queues_claim(_queues, TASKPRIO_MAX);

#ifdef MODULE_PM_LAYERED
#if IS_USED(MODULE_CC2538_RF)
    /* unblock modes that have RAM retention */
    pm_unblock(PM_NUM_MODES - 2);
#endif
    pm_unblock(PM_NUM_MODES - 1);
#endif

    /* wait for events */
    event_t *event;
    while ((event = _event_wait_multi(_queues, TASKPRIO_MAX))) {
        debugpins_task_set();
        event->handler(event);
        /* remove from task list */
        memarray_free(&_scheduler_vars.memarray, event);
#if SCHEDULER_DEBUG_ENABLE
        scheduler_dbg.numTasksCur--;
#endif
        debugpins_task_clr();
    }
}

void scheduler_push_task(task_cbt cb, task_prio_t prio)
{
    unsigned state = irq_disable();

    if(IS_USED(MODULE_PM_LAYERED)) {
        _pm_block();
    }

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
