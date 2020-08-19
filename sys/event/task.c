/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       event_task implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */


#include "stdio.h"
#include "event/task.h"

static int _event_task_cmp(clist_node_t *a, clist_node_t *b)
{
    event_task_t *event_task_a = (event_task_t *) a;
    event_task_t *event_task_b = (event_task_t *) b;
    return event_task_a->prio - event_task_b->prio;
}

static inline event_task_t * _event_task_get_highest(event_queue_t *queue)
{
    event_task_t * head = (event_task_t *) clist_lpeek(&queue->event_list);
    event_task_t * tmp = head;
    event_task_t * result = head;

    if(head == NULL){
        return NULL;
    }

    while ((event_task_t *) tmp->super.list_node.next != head) {
        event_task_t *next = (event_task_t *) tmp->super.list_node.next;
        if (result->prio > next->prio) {
            result = next;
        }
        tmp = next;
    }
    return (event_task_t *) clist_remove(&queue->event_list, &result->super.list_node);
}

void event_task_init(event_task_t *event_task, event_handler_t handler, int prio)
{
    event_task->super.handler = handler;
    event_task->prio = prio;
}

void event_task_sort(event_queue_t *queue)
{
    clist_sort(&queue->event_list, _event_task_cmp);
}

event_task_t *event_task_wait(event_queue_t *queue)
{
    assert(queue);
    event_task_t *result;

    do {
        unsigned state = irq_disable();
        result = _event_task_get_highest(queue);
        irq_restore(state);
        if (result == NULL) {
            thread_flags_wait_any(THREAD_FLAG_EVENT);
        }
    } while (result == NULL);

    return result;
}
