/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dw1000
 * @{
 *
 * @file
 * @brief       Device driver implementation for the driver_dw1000
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "iolist.h"
#include "xtimer.h"

#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "dw1000.h"
#include "dw1000_hal.h"
#include "dw1000_netdev.h"
#include "libdw1000.h"


#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif
#include "log.h"

static int _send(netdev_t *netdev, const iolist_t *iolist);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static void _isr(netdev_t *netdev);
static int _init(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len);

const netdev_driver_t dw1000_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _irq_handler(void *arg)
{
    netdev_trigger_event_isr(arg);
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    (void) netdev;
    (void) iolist;
    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void) netdev;
    (void) buf;
    (void) len;
    (void) info;
    return 0;
}

static int _init(netdev_t *netdev)
{
    dw1000_t *dev = (dw1000_t *)netdev;

    /* init cs pin */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);

    /* init reset pin */
    if (dev->params.reset_pin != GPIO_UNDEF) {
        gpio_init(dev->params.reset_pin, GPIO_IN);
    }

    /* init irq pin */
    if (dev->params.irq_pin != GPIO_UNDEF) {
        gpio_init_int(dev->params.irq_pin, GPIO_IN, GPIO_RISING,
                      _irq_handler, dev);
    }
    /* init libdw */
    LOG_DEBUG("[dw1000]: configure DWM1000\n");
    dwInit(&(dev->dev), (dwOps_t*) &dw1000_ops);

    /* wakeup device*/
    dw1000_wake(dev);

    /* configure */
    int result = dwConfigure(&(dev->dev));

    if (result) {
        LOG_DEBUG("[dw1000] error: %s\n", dwStrError(result));
        return -1;
    } else {
        LOG_DEBUG("[dw1000]: OK\n");
    }

    return 0;
}


static void _isr(netdev_t *netdev)
{
    (void) netdev;
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    (void) netdev;
    (void) opt;
    (void) val;
    (void) max_len;
    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    (void) netdev;
    (void) opt;
    (void) val;
    (void) len;
    return 0;
}

