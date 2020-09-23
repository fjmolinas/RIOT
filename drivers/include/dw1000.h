/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_dw1000 driver_dw1000
 * @ingroup     drivers_drivers_netdev
 * @brief       Decawave DW1000 UWB radio driver
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef DW1000_H
#define DW1000_H

#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "dw1000/dw1000_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct dw1000_dev_cfg dw1000_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    netdev_ieee802154_t netdev;        /**< Netdev parent struct */
    dw1000_dev_instance_t inst;         /**< dwDevice parent struct */
} dw1000_t;

#ifdef __cplusplus
}
#endif

#endif /* DW1000_H */
/** @} */
