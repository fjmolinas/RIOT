/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) event queue wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef MYNEWT_EVENTQ_H
#define MYNEWT_EVENTQ_H

#include <mynewt/types.h>

#include "event/callback.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl event wrapper
 */
struct mynewt_event
{
    event_callback_t e; /**< the event callback */
    void *arg;          /**< the event argument */
};

/**
 * @brief dpl event queue wrapper
 */
struct mynewt_eventq
{
    event_queue_t q;    /**< the event queue */
};

/**
 * @brief dpl event callback function
 */
typedef void mynewt_event_fn(struct mynewt_event *ev);

/**
 * @brief   Init a event
 *
 * @param[in]   ev      pointer to event to set
 * @param[in]   fn      event callback function
 * @param[in]   arg     event argument
 */
static inline void mynewt_event_init(struct mynewt_event *ev, mynewt_event_fn * fn,
                                 void *arg)
{
    /*
     * Need to clear list_node manually since init function below does not do
     * this.
     */
    ev->e.super.list_node.next = NULL;
    event_callback_init(&ev->e, (void(*)(void *))fn, ev);
    ev->arg = arg;
}

/**
 * @brief   Check if event is in queue
 *
 * @param[in]   ev      event to check
 *
 * @return  true if event is queues, false otherwise
 */
static inline bool mynewt_event_is_queued(struct mynewt_event *ev)
{
    return (ev->e.super.list_node.next != NULL);
}

/**
 * @brief   Runs an event
 *
 * @param[in]   ev      event to run
 */
static inline void *mynewt_event_get_arg(struct mynewt_event *ev)
{
    return ev->arg;
}

/**
 * @brief   Set the vent arg
 *
 * @param[in]   ev      event
 * @param[in]   arg     arg to set event
 */
static inline void mynewt_event_set_arg(struct mynewt_event *ev, void *arg)
{
    ev->arg = arg;
}

/**
 * @brief   Runs an event
 *
 * @param[in]   ev      event to run
 */
static inline void mynewt_event_run(struct mynewt_event *ev)
{
    ev->e.super.handler(&ev->e.super);
}

/**
 * @brief   Initialize the event queue
 *
 * @param[in]   evq     The event queue to initialize
 */
static inline void mynewt_eventq_init(struct mynewt_eventq *evq)
{
    event_queue_init_detached(&evq->q);
}

/**
 * @brief   Check whether the event queue is initialized.
 *
 * @param[in]   evq     the event queue to check
 */
static inline int mynewt_eventq_inited(struct mynewt_eventq *evq)
{
    return evq->q.waiter != NULL;
}

/**
 * @brief   Deinitialize an event queue
 *
 * @note    Not supported in RIOT
 *
 * @param[in]   evq     the event queue to deinit
 */
static inline void mynewt_eventq_deinit(struct mynewt_eventq *evq)
{
    (void) evq;
    /* Can't deinit an eventq in RIOT */
}

/**
 * @brief   Get next event from event queue, blocking.
 *
 * @param[in]   evq     the event queue to pull an event from
 *
 * @return  the event from the queue
 */
static inline struct mynewt_event * mynewt_eventq_get(struct mynewt_eventq *evq)
{
    if (evq->q.waiter == NULL) {
        event_queue_claim(&evq->q);
    }

    return (struct mynewt_event *) event_wait(&evq->q);
}

/**
 * @brief   Get next event from event queue, non-blocking
 *
 * @return  event from the queue, or NULL if none available.
 */
static inline struct mynewt_event * mynewt_eventq_get_no_wait(struct mynewt_eventq *evq)
{
    if (evq->q.waiter == NULL) {
        event_queue_claim(&evq->q);
    }

    return (struct mynewt_event *) event_get(&evq->q);
}

/**
 * @brief   Put an event on the event queue.
 *
 * @param[in]   evq     event queue
 * @param[in]   ev      event to put in queue
 */
static inline void mynewt_eventq_put(struct mynewt_eventq *evq, struct mynewt_event *ev)
{
    event_post(&evq->q, &ev->e.super);
}

/**
 * @brief   Remove an event from the queue.
 *
 * @param[in]   evq     event queue to remove the event from
 * @param[in]   ev      event to remove from the queue
 */
static inline void mynewt_eventq_remove(struct mynewt_eventq *evq, struct mynewt_event *ev)
{
    event_cancel(&evq->q, &ev->e.super);
}

/**
 * @brief   Gets and runs an event from the queue callback.
 *
 * @param[in]   evq     The event queue to pull the item off.
 */
static inline void mynewt_eventq_run(struct mynewt_eventq *evq)
{
    struct mynewt_event *ev = mynewt_eventq_get(evq);
    mynewt_event_run(ev);
}

/**
 * @brief   Check if queue is empty
 *
 * @param[in]   evq     the event queue to check
 *
 * @return      true    if empty, false otherwise
 */
static inline bool mynewt_eventq_is_empty(struct mynewt_eventq *evq)
{
    return clist_count(&(evq->q.event_list)) == 0;
}

#ifdef __cplusplus
}
#endif

#endif /* MYNEWT_EVENTQ_H */
