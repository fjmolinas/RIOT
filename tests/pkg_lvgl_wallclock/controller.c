/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @ingroup
 * @brief
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdint.h>
#include "controller.h"

#include "event/timeout.h"
#include "ztimer.h"
#include "timex.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_DEBUG
#endif
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

static controller_t _controller = { .lock = MUTEX_INIT };
static event_queue_t *_queue;
static void _controller_handler(event_t *event);
static event_t _status_change = {.handler = _controller_handler, .list_node.next=NULL};
static event_timeout_t _timeout;

void _controller_handler(event_t *event)
{
    (void)event;
    if(!mutex_trylock(&_controller.lock)) {
        LOG_DEBUG("[controller]: failed to acquire lock\n");
        controller_status_changed();
    }
    if (_controller.status & CONTROLLER_STATUS_BRIGHTNESS) {
        LOG_DEBUG("[controller]: brightness changed: %d\n", _controller.brightness);
        _controller.status &= ~CONTROLLER_STATUS_BRIGHTNESS;
    }
    if (_controller.status & CONTROLLER_STATUS_BLE_AUTO_CONN) {
        LOG_DEBUG("[controller]: enable autoconn\n");
    }
    if (_controller.status & CONTROLLER_STATUS_BLE_TIME_ADV) {
        LOG_DEBUG("[controller]: enable time advertiser\n");
    }
    if (_controller.status & CONTROLLER_STATUS_BLE_STDIO) {
        LOG_DEBUG("[controller]: enable ble stdio\n");
    }
    mutex_unlock(&_controller.lock);
}

void controller_init(event_queue_t *queue)
{
    _queue = queue;
    _controller.brightness = 100;
    _controller.status = 0;
    event_timeout_ztimer_init(&_timeout, ZTIMER_MSEC, _queue, &_status_change);
}

void controller_status_changed(void)
{
    event_timeout_set(&_timeout, MS_PER_SEC);
}

controller_t* controller_adquire(void)
{
    LOG_DEBUG("[controller]: lock\n");
    mutex_lock(&_controller.lock);

    return &_controller;
}

int controller_release(void)
{
    LOG_DEBUG("[controller]: unlock\n");
    mutex_unlock(&_controller.lock);
    return 0;
}
