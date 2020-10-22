/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       OpenWSN bootstraping functions implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "scheduler.h"
#include "openstack.h"
#include "radio.h"

#include "openwsn.h"
#include "openwsn_board.h"
#include "openwsn_radio.h"

#include "event.h"
#include "event/timeout.h"
#include "event/callback.h"

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#define LOG_LEVEL LOG_ALL
#include "log.h"

#define OPENWSN_SCHED_NAME            "openwsn-sched"
#define OPENWSN_SCHED_PRIO            (THREAD_PRIORITY_MAIN - 4)
#define OPENWSN_SCHED_STACKSIZE       (2048)

#define OPENWSN_TSCH_NAME             "openwsn-tsch"
#define OPENWSN_TSCH_PRIO             (THREAD_PRIORITY_MAIN - 5)
#define OPENWSN_TSCH_STACKSIZE        (2048)

#ifdef MODULE_OPENWSN_RADIO_NETDEV
#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif
#endif

/* tsch stack */
static char _tsch_stack[OPENWSN_TSCH_STACKSIZE];
/* scheduler stack */
static char _sched_stack[OPENWSN_SCHED_STACKSIZE];

/* tsch event queues, radio and timer generated */
static event_queue_t _queues[2] = {
    EVENT_QUEUE_INIT_DETACHED, EVENT_QUEUE_INIT_DETACHED
};

static kernel_pid_t _sched_pid = KERNEL_PID_UNDEF;
static kernel_pid_t _tsch_pid = KERNEL_PID_UNDEF;

void* _radio_init_dev(void)
{
    void* dev = NULL;
#ifdef MODULE_OPENWSN_RADIO_NETDEV
    #ifdef MODULE_AT86RF2XX
        dev = &at86rf2xx_dev.netdev.netdev;
        at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0], 0);
    #endif
#else
    #ifdef MODULE_CC2538_RF
        extern ieee802154_dev_t cc2538_rf_dev;
        dev = &cc2538_rf_dev;
        cc2538_init();
    #endif
    #ifdef MODULE_NRF802154
        extern ieee802154_dev_t nrf802154_hal_dev;
        dev = &nrf802154_hal_dev;
        nrf802154_init();
    #endif
#endif
    return dev;
}


static void *_sched_event_loop(void *arg)
{
    (void)arg;

    LOG_DEBUG("[openwsn]: init scheduler\n");
    scheduler_init();
    LOG_DEBUG("[openwsn]: init openstack\n");
    /* Disable IRQ while scheduler is not ready to start */
    unsigned irq_state = irq_disable();
    openstack_init();
    LOG_DEBUG("[openwsn]: start scheduler loop\n");
    scheduler_start(irq_state);

    return NULL;
}

static void *_tsch_event_loop(void *arg)
{
    (void)arg;

    LOG_DEBUG("[openwsn]: init tsch thread\n");

    event_queues_claim(_queues, 2);
    event_t *event;

    while ((event = event_wait_multi(_queues, 2))) {
        // LOG_DEBUG("[openwsn]: event\n");
        event->handler(event);
    }

    return NULL;
}

int openwsn_bootstrap(void)
{
    LOG_DEBUG("[openwsn]: init RIOT board\n");
    board_init_openwsn();

    LOG_DEBUG("[openwsn]: init radio\n");
    void* dev = _radio_init_dev();
    if (openwsn_radio_init(dev)) {
        LOG_ERROR("[openwsn]: failed to init radio\n");
        return -1;
    }

    LOG_DEBUG("[openwsn]: network thread\n");

    _sched_pid = thread_create(_sched_stack, OPENWSN_SCHED_STACKSIZE, OPENWSN_SCHED_PRIO,
                         THREAD_CREATE_STACKTEST, _sched_event_loop, NULL,
                         OPENWSN_SCHED_NAME);

    if (_sched_pid <= 0) {
        LOG_ERROR("[openwsn]: couldn't create scheduler thread\n");
        return -1;
    }

    _tsch_pid = thread_create(_tsch_stack, OPENWSN_TSCH_STACKSIZE, OPENWSN_TSCH_PRIO,
                              THREAD_CREATE_STACKTEST, _tsch_event_loop, NULL,
                              OPENWSN_TSCH_NAME);

    if (_tsch_pid <= 0) {
        LOG_ERROR("[openwsn]: couldn't create tsch thread\n");
        return -1;
    }

    return 0;
}

kernel_pid_t openwsn_sched_pid(void)
{
    return _sched_pid;
}

kernel_pid_t openwsn_tsch_pid(void)
{
    return _tsch_pid;
}

event_queue_t* openwsn_tsch_radio_evq(void)
{
    return &_queues[0];
}

event_queue_t* openwsn_tsch_timer_evq(void)
{
    return &_queues[0];
}
