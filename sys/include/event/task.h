/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_event
 * @brief       Provides an event with priority type
 *
 *
 * @{
 *
 * @file
 * @brief       Event Task API
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#ifndef EVENT_TASK_H
#define EVENT_TASK_H

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback Event structure definition
 */
typedef struct {
    event_t super;              /**< event_t structure that gets extended   */
    int prio;                   /**< event priority */
} event_task_t;

/**
 * @brief   event callback initialization function
 *
 * @param[out]  event_callback  object to initialize
 * @param[in]   callback        callback to set up
 * @param[in]   arg             callback argument to set up
 */
void event_task_init(event_task_t *event_task, event_handler_t handler, int prio);

/**
 * @brief   Sorts an event_task queue according to event priority
 *
 * Highest priority (lowest value) will be at the head.
 *
 * @param[in]   queue   event queue to sort
 */
void event_task_sort(event_queue_t *queue);

/**
 * @brief   Get next event with the highest priority from queue
 *
 * This function will block until an event becomes available.
 *
 * In order to handle an event retrieved using this function,
 * call event->handler(event).
 *
 * @note can simply call event_wait() if the queue is sorted
 *
 * @warning     There can only be a single waiter on a queue!
 *
 * @param[in]   queue   event queue to get event from
 *
 * @returns     pointer to next event
 */
event_task_t *event_task_wait(event_queue_t *queue);

/**
 * @brief   Callback Event static initializer
 *
 * @param[in]   _cb     callback function to set
 * @param[in]   _arg    arguments to set
 */
#define EVENT_TASK_INIT(_handler, _prio) \
    { \
        .super.handler = _handler, \
        .prio = _prio, \
    }

#ifdef __cplusplus
}
#endif
#endif /* EVENT_TASK_H */
/** @} */
