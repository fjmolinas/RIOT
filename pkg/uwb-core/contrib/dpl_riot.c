/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb
 * @{
 *
 * @file
 * @brief       Glue code for running uwb-core for RIOT
 * *
 * @}
 */

#include <stdatomic.h>

#include "thread.h"
#include "dpl_riot.h"

#include <dpl/dpl_eventq.h>

#define LOG_LEVEL   LOG_DEBUG
#include "log.h"

#ifndef UWB_CORE_STACKSIZE
#define UWB_CORE_STACKSIZE  (THREAD_STACKSIZE_LARGE)
#endif
#ifndef UWB_CORE_PRIO
#define UWB_CORE_PRIO       (THREAD_PRIORITY_MAIN - 5)
#endif

static char _stack_uwb_core[UWB_CORE_STACKSIZE];

static struct dpl_eventq dflt_evq;

atomic_uint dpl_in_critical = 0;

static void _uwb_core_port_init(void)
{
    /* Initialize default event queue */
    dpl_eventq_init(&dflt_evq);
}

static void _uwb_core_port_run(void)
{
    while (1) {
        dpl_eventq_run(&dflt_evq);
    }
}

struct dpl_eventq * dpl_eventq_dflt_get(void)
{
    return &dflt_evq;
}

static void *_uwb_core_thread(void *arg)
{
    (void)arg;

    _uwb_core_port_init();

    _uwb_core_port_run();

    /* never reached */
    return NULL;
}

void uwb_core_riot_init(void)
{
    int res;
    (void)res;

    thread_create(_stack_uwb_core, sizeof(_stack_uwb_core),
                  UWB_CORE_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _uwb_core_thread, NULL,
                  "uwb_core");
}

