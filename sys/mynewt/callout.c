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
#include "mynewt/callout.h"

static void _mynewt_callout_timer_cb(void* arg)
{
    struct mynewt_callout *c = (struct mynewt_callout *) arg;
    assert(c);

    /* post the event if there is a queue, otherwise call the callback
       here */
    if (c->c_q) {
        mynewt_eventq_put(c->c_q, &c->c_e);
    } else {
        c->c_e.e.callback(&c->c_e);
    }
}

void mynewt_callout_init(struct mynewt_callout *c, struct mynewt_eventq *q,
                      mynewt_event_fn *e_cb, void *e_arg)
{
    mynewt_event_init(&c->c_e, e_cb, e_arg);
    c->c_q = q;
    c->timer.callback = _mynewt_callout_timer_cb;
    c->timer.arg = (void*) c;
}

mynewt_error_t mynewt_callout_reset(struct mynewt_callout *c, mynewt_time_t ticks)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    xtimer_set(&(c->timer), xtimer_usec_from_ticks(val));
    return MYNEWT_OK;
}

void mynewt_callout_stop(struct mynewt_callout *c)
{
    xtimer_remove(&(c->timer));
}
