/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       event task test application
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdatomic.h>

#include "test_utils/expect.h"
#include "thread.h"
#include "event.h"
#include "event/task.h"

#define STACKSIZE               THREAD_STACKSIZE_DEFAULT
#define PRIO                    (THREAD_PRIORITY_MAIN - 1)

static char stack[STACKSIZE];

static void cb(event_t *arg);
static void cb_end(event_t *arg);

static event_task_t evt_1   = { .super.handler = cb, .prio =  1};
static event_task_t evt_2   = { .super.handler = cb, .prio =  2};
static event_task_t evt_3   = { .super.handler = cb, .prio =  3};
static event_task_t evt_end = { .super.handler = cb_end, .prio =  4};

static atomic_uint prio;

static void cb_end(event_t *arg)
{
    event_task_t * event = (event_task_t *) arg;
    printf("triggered event %p, prio: %d\n", (void *)event, event->prio);
    expect(atomic_load(&prio) < (unsigned int) event->prio);
    atomic_store(&prio, (unsigned int) event->prio);
    puts("\n[SUCCESS]");
}

static void cb(event_t *arg)
{
    event_task_t * event = (event_task_t *) arg;
    printf("triggered event %p, prio: %d\n", (void *)event, event->prio);
    expect(atomic_load(&prio) < (unsigned int) event->prio);
    atomic_store(&prio, (unsigned int) event->prio);
}

static void *claiming_thread(void *arg)
{
    event_queue_t *q = arg;

    event_queue_claim(q);
    event_loop(q);

    return NULL;
}

int main(void)
{
    event_queue_t dqueue = EVENT_QUEUE_INIT_DETACHED;
    event_queue_init_detached(&dqueue);

    puts("\ntest event_task_sort()\n");
    atomic_store(&prio, 0);

    event_post(&dqueue, (event_t*)  &evt_1);
    event_post(&dqueue, (event_t*)  &evt_3);
    event_post(&dqueue, (event_t*)  &evt_2);
    event_task_sort(&dqueue);

    thread_create(stack, sizeof(stack), PRIO, THREAD_CREATE_STACKTEST,
                  claiming_thread, &dqueue, "ct");


    puts("\ntest event_task_wait()\n");
    atomic_store(&prio, 0);
    event_queue_t queue = EVENT_QUEUE_INIT;

    event_post(&queue, (event_t*) &evt_end);
    event_post(&queue, (event_t*) &evt_1);
    event_post(&queue, (event_t*) &evt_3);
    event_post(&queue, (event_t*) &evt_2);

    event_task_t *event;

    while ((event = event_task_wait(&queue))) {
        event->super.handler((event_t*) event);
        event->super.handler = NULL;
    }

    return 0;
}
