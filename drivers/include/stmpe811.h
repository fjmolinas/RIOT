/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_stmpe811 STMPE811 Touchscreen Controller
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver interface for the STMPE811 touchscreen controller
 *
 * @{
 *
 * @file
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef STMPE811_H
#define STMPE811_H

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Status and error return codes
 */
enum {
    STMPE811_OK = 0,                /**< Everything was fine */
    STMPE811_ERR_I2C                /**< Error initializing the I2C bus */
};

/**
 * @brief  Orientation enum
 */
typedef enum {
	stmpe811_portrait_1,
	stmpe811_portrait_2,
	stmpe811_landscape_1,
	stmpe811_landscape_2,
} stmpe811_orientation_t;

/**
 * @brief  Touch state enum
 */
typedef enum {
	STMPE811_STATE_PRESSED,
	STMPE811_STATE_RELEASED,
	STMPE811_STATE_OK,
	STMPE811_STATE_ERROR,
} stmpe811_state_t;

/**
 * @brief  Data structure
 */
typedef struct {
	uint16_t x;
	uint16_t y;
	stmpe811_state_t pressed;
	stmpe811_state_t last_pressed;
	stmpe811_orientation_t orientation;
} stmpe811_data_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
     i2c_t i2c;                     /**< I2C device which is used */
     uint8_t addr;                  /**< Device I2C address */
} stmpe811_params_t;

/**
 * @brief   Device descriptor for the STMPE811 sensor
 */
typedef struct {
    stmpe811_params_t params;       /**< Device parameters */
} stmpe811_t;

/**
 * @brief   Initialize the given STMPE811 device
 *
 * @param[out] dev          Initialized device descriptor of STMPE811 device
 * @param[in]  params       The parameters for the STMPE811 device
 *
 * @return                  VEML6070_OK on success
 * @return                  VEML6070_ERR_I2C if given I2C is not enabled in board config
 */
int stmpe811_init(stmpe811_t *dev, const stmpe811_params_t * params);

void stmpe811_reset(const stmpe811_t *dev);
void stmpe811_enable_ts(const stmpe811_t *dev);
void stmpe811_disable_ts(const stmpe811_t *dev);
void stmpe811_enable_gpio(const stmpe811_t *dev);
void stmpe811_disable_gpio(const stmpe811_t *dev);
void stmpe811_enable_tsc(const stmpe811_t *dev);
void stmpe811_disable_tsc(const stmpe811_t *dev);
void stmpe811_enable_adc(const stmpe811_t *dev);
void stmpe811_disable_adc(const stmpe811_t *dev);
void stmpe811_enable_fifo_of(const stmpe811_t *dev);
void stmpe811_enable_fifo_th(const stmpe811_t *dev);
void stmpe811_enable_fifo_touch_det(const stmpe811_t *dev);
void stmpe811_set_adc_sample(const stmpe811_t *dev, uint8_t sample);
void stmpe811_set_adc_resolution(const stmpe811_t *dev, uint8_t res);
void stmpe811_set_adc_freq(const stmpe811_t *dev, uint8_t freq);
void stmpe811_set_gpio_af(const stmpe811_t *dev, uint8_t af);
void stmpe811_set_tsc(const stmpe811_t *dev, uint8_t tsc);
void stmpe811_set_fifo_th(const stmpe811_t *dev, uint8_t th);
void stmpe811_reset_fifo(const stmpe811_t *dev);
void stmpe811_set_tsc_fraction_z(const stmpe811_t *dev, uint8_t z);
void stmpe811_set_tsc_i_drive(const stmpe811_t *dev, uint8_t limit);
void stmpe811_set_int_sta(const stmpe811_t *dev, uint8_t status);
void stmpe811_enable_interrupts(const stmpe811_t *dev);
void stmpe811_read_xy(const stmpe811_t *dev, uint16_t *x, uint16_t *y);
stmpe811_state_t stmpe811_read_touch(const stmpe811_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* STMPE811_H */
/** @} */
