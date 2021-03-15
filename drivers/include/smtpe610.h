/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_smtpe610 sensors
 * @ingroup     drivers_sensors
 * @brief       S-Touch®: advanced touchscreen controlle driver
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef SMTPE610_H
#define SMTPE610_H

#include "errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Touch position structure
 */
typedef struct {
    uint16_t x;                         /**< X position */
    uint16_t y;                         /**< Y position */
} stmpe610_touch_position_t;

/**
 * @brief   Signature of touch event callback triggered from interrupt
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*stmpe610_event_cb_t)(void *arg);
/**
 * @brief   Device initialization parameters
 */
typedef struct {
#if IS_USED(MODULE_STMPE610_SPI)
    /* SPI configuration */
    spi_t spi;                          /**< SPI bus */
    spi_clk_t clk;                      /**< clock speed for the SPI bus */
    gpio_t cs;                          /**< chip select pin */
#else
    /* I2C details */
    i2c_t i2c_dev;                      /**< I2C device which is used */
    uint8_t i2c_addr;                   /**< I2C address */
#endif
    gpio_t int_pin;                    /**< Touch screen interrupt pin */
    uint16_t xmax;                     /**< Touch screen max X position */
    uint16_t ymax;                     /**< Touch screen max Y position */
} smtpe610_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
#ifdef MODULE_TOUCH_DEV
    touch_dev_t *dev;                   /**< Pointer to the generic touch device */
#endif
    stmpe610_event_cb_t cb;             /**< Configured IRQ event callback */
    void *cb_arg;                       /**< Extra argument for the callback */
    smtpe610_params_t params;
} smtpe610_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int smtpe610_init(smtpe610_t *dev, const smtpe610_params_t *params);

/**
 * @brief   Initialize the given STMPE610 device
 *
 * @param[inout] dev        Device descriptor of the STMPE610
 * @param[in] params        Initialization parameters of the STMPE610 device
 * @param[in] cb            Callback function called on touch interrupts
 * @param[in] arg           Context argument used in callback function
 *
 * @return                  0 on success
 * @return                  -ENODEV when no valid device
 * @return                  -EIO when software reset failed
 * @return                  -EPROTO on any I2C error
 */
int stmpe610_init(stmpe610_t *dev, const stmpe610_params_t * params,
                  stmpe610_event_cb_t cb, void *arg);

/**
 * @brief   Read the touch position
 *
 * @param[in] dev           Device descriptor of the STMPE610
 * @param[out] position     Touch position
 *
 * @return                  0 on success
 * @return                  -EPROTO on any I2C error
 */
int stmpe610_read_touch_position(stmpe610_t *dev, stmpe610_touch_position_t *position);

/**
 * @brief   Read the touch state (pressed or released)
 *
 * @param[in] dev           Device descriptor of the STMPE610
 * @param[out] state        Touch state
 *
 * @return                  0 on success
 * @return                  -EPROTO on any I2C error
 */
int stmpe610_read_touch_state(const stmpe610_t *dev, stmpe610_touch_state_t *state);


#ifdef __cplusplus
}
#endif

#endif /* SMTPE610_H */
/** @} */
