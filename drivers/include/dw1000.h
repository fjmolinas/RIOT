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

#include "libdw1000Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    spi_mode_t mode;        /**< SPI mode */
    gpio_t irq_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} dw1000_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    netdev_ieee802154_t netdev;        /**< Netdev parent struct */
    dwDevice_t dev;                    /**< dwDevice parent struct */
    dw1000_params_t params;            /**< Device driver parameters */
} dw1000_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 */
void dw1000_setup(dw1000_t *dev, const dw1000_params_t *params);

/**
 * @brief   Trigger a hardware reset
 *
 * @param[in,out] dev       device to reset
 */
void dw1000_reset(dw1000_t *dev);

/**
 * @brief   If not awake, wake up device
 *
 * @param[in,out] dev       device to reset
 *
 * @return                  0 on success
 */
int dw1000_wake(dw1000_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* DW1000_H */
/** @} */
