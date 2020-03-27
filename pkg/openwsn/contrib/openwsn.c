/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#include "scheduler.h"
#include "openstack.h"
#include "radio.h"

#include "board_ow.h"
#include "radio_ow.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_AT86RF215
#include "at86rf215.h"
#include "at86rf215_params.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define OPENWSN_SCHED_NAME            "openwsn"
#define OPENWSN_SCHED_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define OPENWSN_SCHED_STACKSIZE       (2048)

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif

#ifdef MODULE_AT86RF215
static at86rf215_t at86rf215_24_dev;
#endif

static char _stack[OPENWSN_SCHED_STACKSIZE];

static kernel_pid_t _pid = KERNEL_PID_UNDEF;

kernel_pid_t openwsn_get_pid(void) {
    return _pid;
}
static void *_event_loop(void *arg);

int openwsn_bootstrap(void)
{
    DEBUG("[openwsn_bootstrap]: init RIOT board\n");
    board_init_openwsn();

#ifdef MODULE_AT86RF2XX
    netdev_t *netdev = (netdev_t *)&at86rf2xx_dev.netdev.netdev;
    at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
#endif

#ifdef MODULE_AT86RF215
    netdev_t *netdev = (netdev_t *)&at86rf215_24_dev.netdev.netdev;
    at86rf215_setup(NULL, &at86rf215_24_dev, &at86rf215_params[0]);
#endif

#ifdef MODULE_OPENWSN_RADIO
    DEBUG("[openwsn_bootstrap]: init radio\n");
    if (openwsn_radio_init(netdev)) {
        DEBUG("[openwsn_bootstrap]: failed to init radio\n");
        return -1;
    }
#endif

    DEBUG("[openwsn_bootstrap]: network thread\n");
    _pid = thread_create(_stack, OPENWSN_SCHED_STACKSIZE, OPENWSN_SCHED_PRIO,
                            THREAD_CREATE_STACKTEST, _event_loop, NULL,
                            OPENWSN_SCHED_NAME);
    if (_pid <= 0) {
        DEBUG("[openwsn_bootstrap]: couldn't create thread\n");
        return -1;
    }

    return _pid;
}

static void *_event_loop(void *arg)
{
    (void) arg;

    DEBUG("[openwsn_bootstrap]: init scheduler\n");
    scheduler_init();
    DEBUG("[openwsn_bootstrap]: init openstack\n");
    openstack_init();
    DEBUG("[openwsn_bootstrap]: start scheduler loop\n");
    scheduler_start();

    return NULL;
}
