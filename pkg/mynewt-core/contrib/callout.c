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
 * @brief       uwb-core DPL (Decawave Porting Layer) callout
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>

#include "xtimer.h"
#include "os/callout.h"

static void _os_callout_timer_cb(void* arg)
{
    struct os_callout *c = (struct os_callout *) arg;
    assert(c);

    /* post the event if there is a queue, otherwise call the callback
       here */
    if (c->c_q) {
        os_eventq_put(c->c_q, &c->c_e);
    } else {
        c->c_e.e.callback(&c->c_e);
    }
}

void os_callout_init(struct os_callout *c, struct os_eventq *q,
                      os_event_fn *e_cb, void *e_arg)
{
    os_event_init(&c->c_e, e_cb, e_arg);
    c->c_q = q;
    c->timer.callback = _os_callout_timer_cb;
    c->timer.arg = (void*) c;
}

os_error_t os_callout_reset(struct os_callout *c, os_time_t ticks)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    xtimer_set(&(c->timer), xtimer_usec_from_ticks(val));
    return OS_OK;
}

void os_callout_stop(struct os_callout *c)
{
    xtimer_remove(&(c->timer));
}
