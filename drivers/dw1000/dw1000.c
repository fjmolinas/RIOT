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

#include "dw1000.h"
#include "dw1000_constants.h"
#include "dw1000_params.h"

#include "dw1000_hal.h"

#include "libdw1000.h"

#include "periph/gpio.h"
#include "xtimer.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif
#include "log.h"

#define DW1000_TRST_OK      (50U) /* nominal value of 50ns */
#define CLOCK_CORECLOCK_P_N (1000000000 / CLOCK_CORECLOCK) /* period in ns*/


static int _dw1000_wake(dw1000_t *dev)
{
    if (dwGetDeviceId(&(dev->dev)) != 0xdeca0130) {
        /* keep SPI CS line low for 500 microseconds */
        gpio_clear(dev->params.cs_pin);
        xtimer_usleep(500);
        gpio_set(dev->params.cs_pin);
        /* give device time to init */
        xtimer_usleep(5000);

        if (dwGetDeviceId(&(dev->dev)) != 0xdeca0130) {
            LOG_DEBUG("[dw1000]: failed to wakeup device\n");
            return -1;
        }
    }
    else {
        LOG_DEBUG("[dw1000]: device is already awake\n");
    }

    return 0;
}

int dw1000_init(dw1000_t *dev, const dw1000_params_t *params)
{
    LOG_DEBUG("[dw1000]: initializing DWM1000\n");
    /* initialize device descriptor */
    dev->params = *params;

    /* init cs pin */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);

    /* init reset pin */
    if (dev->params.reset_pin != GPIO_UNDEF) {
        gpio_init(dev->params.reset_pin, GPIO_IN);
    }

    /* init irq pin */
    if (dev->params.irq_pin != GPIO_UNDEF) {
        /* not used by the implementation */
    }

    /* init libdw */
    LOG_DEBUG("[dw1000]: configure DWM1000\n");
    dwInit(&(dev->dev), (dwOps_t*) &dw1000_ops);

    /* wakeup device*/
    _dw1000_wake(dev);

    /* configure */
    int result = dwConfigure(&(dev->dev));

    if (result) {
        LOG_DEBUG("[dw1000]: ERROR %s\n", dwStrError(result));
        return -1;
    } else {
        LOG_DEBUG("[dw1000]: OK\n");
    }
    return 0;
}

void dw1000_reset(dw1000_t *dev)
{
    if (dev->params.reset_pin != GPIO_UNDEF) {
        LOG_DEBUG("[dw1000]: hard reset\n");
        gpio_init(dev->params.reset_pin, GPIO_OD);
        gpio_clear(dev->params.reset_pin);
        for (uint32_t i = 0; i < (CLOCK_CORECLOCK_P_N * DW1000_TRST_OK); i++) {
            /* Make sure for loop is not optimized out */
            __asm__ ("");
        }
        gpio_set(dev->params.reset_pin);
        /* DW1000-V2.12 5.6.3.1:
           RSTn should never be driven high by an external source. */
        gpio_init(dev->params.reset_pin, GPIO_IN);
    }
}
