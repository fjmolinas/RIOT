/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mics6814 driver_mics6814
 * @ingroup     drivers_sensors
 * @brief       Driver for the MICS6814 Gas sensor
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef MICS6814_H
#define MICS6814_H

#include "periph/adc.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    adc_t aox;              /**< ADC line connected to device AOUT pin. */
    adc_t ared;             /**< ADC line connected to device AOUT pin. */
    adc_t anh3;             /**< ADC line connected to device AOUT pin. */
    adc_res_t adc_res;      /**< ADC resolution. */
} mics6814_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    mics6814_params_t params;
} mics6814_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int mics6814_init(mics6814_t *dev, const mics6814_params_t *params);

/**
 * @brief   Read a raw Oxidizing (CO) ADC value
 *
 * @param[in]       dev     Device descriptor.
 *
 * @return  Oxidizing (CO) concentration value
 */
int32_t mics6814_read_raw_ox(mics6814_t *dev);

/**
 * @brief   Read a raw Reducing (NO2) ADC value
 *
 * @param[in]       dev     Device descriptor.
 *
 * @return  Reducing (NO2) concentration value
 */
int32_t mics6814_read_raw_red(mics6814_t *dev);

/**
 * @brief   Read a raw NH3 ADC value
 *
 * @param[in]       dev     Device descriptor.
 *
 * @return  NH3 concentration value
 */
int32_t mics6814_read_raw_nh3(mics6814_t *dev);

#if IS_USED(MICS6814_FULL)
/**
 * @brief   Read a raw NH3 ADC value
 *
 * @param[in]       dev     Device descriptor.
 *
 * @return  NH3 concentration value
 */
float mics6814_read_co(mics6814_t *dev);

float mics6814_read_no2(mics6814_t *dev);

float mics6814_read_nh3(mics6814_t *dev);

float mics6814_read_c3h8(mics6814_t *dev);

float mics6814_read_c4h10(mics6814_t *dev);
float mics6814_read_ch4(mics6814_t *dev);

float mics6814_read_h2(mics6814_t *dev);

float mics6814_read_ch50h(mics6814_t *dev);
#endif

#ifdef __cplusplus
}
#endif

#endif /* MICS6814_H */
/** @} */
