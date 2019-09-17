
/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sm_pwm_01c SM_PWM_01C dust sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for Amphenol SM_PWM_01C infrared dust sensor
 * @{
 *
 * @file
 * @brief       SM_PWM_01C Device Driver
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef SM_PWM_01C_H
#define SM_PWM_01C_H

#include <inttypes.h>

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup    drivers_sm_pwm_01c_conf  SM_PWM_01C  compile configurations
 * @ingroup     drivers_sm_pwm_01c
 * @ingroup     config
 * @{
 */

/**
 * @def     SM_PWM_01C_SAMPLE_TIME
 *
 * @brief   Frequency at witch LPO % is calculated
 */
#ifndef SM_PWM_01C_SAMPLE_TIME
#define SM_PWM_01C_SAMPLE_TIME           (100*US_PER_MS)
#endif

/**
 * @def     SM_PWM_01C_MOVING_AVERAGE
 *
 * @brief   Define this variable to use moving average instead of weighted average
 */
#if defined(DOXYGEN)
#define SM_PWM_01C_MOVING_AVERAGE
#endif

#ifdef SM_PWM_01C_MOVING_AVERAGE
/**
 * @def     SM_PWM_01C_WINDOW_TIME
 *
 * @brief   Length in time of the measuring window
 */
#ifndef SM_PWM_01C_WINDOW_TIME
#define SM_PWM_01C_WINDOW_TIME           (10*US_PER_SEC)
#endif
/*
 * @brief   Length in time of the measuring window
 */
#define SM_PWM_01C_BUFFER_LEN            (SM_PWM_01C_WINDOW_TIME / \
                                          SM_PWM_01C_SAMPLE_TIME)
#else
/**
 * @def     SM_PWM_01C_WEIGHT
 *
 * @brief   Weight of the weighted average filter
 */
#ifndef SM_PWM_01C_WEIGHT
#define SM_PWM_01C_WEIGHT                (5)
#endif
#endif

/** @} */

/**
 * @brief   Circular buffer holding moving average values
 */
#ifdef SM_PWM_01C_MOVING_AVERAGE
typedef struct {
    uint16_t * buf;                   /**< pointer to buffer */
    int head;                         /**< current buffer head */
    int len;                          /**< buffer len */
} circ_buf_t;
#endif

/**
 * @brief       Status and error return codes
 */
enum {
    SM_PWM_01C_OK            =  0,           /**< everything was fine */
    SM_PWM_01C_ERR_GPIO      = -1,           /**< error initializing the GPIO's*/
    SM_PWM_01C_ERR_OTHER     = -999,         /**< fatal error */
};

/**
 * @brief   Parameters for the SM_PWM_01c sensor
 *
 * These parameters are needed to configure the device at startup.
 */
typedef struct {
    gpio_t tsp_pin;              /**< Low Pulse Signal Output (P1)
                                      of small Particle, active low */
    gpio_t tlp_pin;              /**< Low Pulse Signal Output (P2)
                                      of large Particle, active low */
} sm_pwm_01c_params_t;

/**
 * @brief   LPO and concentration (ug/m3) values for small and large particles
 */
typedef struct {
    uint32_t tsp_lpo;            /**< Small particle low Pulse active time us */
    uint32_t tlp_lpo;            /**< Large Particle low Pulse active time us */
#ifdef SM_PWM_01C_MOVING_AVERAGE
                                 /**< Small particle concentration buffer in ug/m3 */
    uint16_t tlp_conc_buf[SM_PWM_01C_BUFFER_LEN];
                                 /**< Large particle concentration buffer in ug/m3 */
    uint16_t tsp_conc_buf[SM_PWM_01C_BUFFER_LEN];
#else
    uint16_t tsp_conc;           /**< Small particle concentration ug/m3 */
    uint16_t tlp_conc;           /**< Large particle concentration ug/m3 */
#endif
} sm_pwm_01c_values_t;

/**
 * @brief   Device descriptor for the SM_PWM_01c sensor
 */
typedef struct {
    sm_pwm_01c_params_t params;  /**< Device driver parameters */
    sm_pwm_01c_values_t values;  /**< Lpo and concentration values */
#ifdef SM_PWM_01C_MOVING_AVERAGE
    circ_buf_t tsp_circ_buf;  /**< Small particle moving average values */
    circ_buf_t tlp_circ_buf;  /**< Large particle moving average values */
#endif
} sm_pwm_01c_t;


/**
 * @brief       Initialize the given SM_PWM_01C device
 *
 * @param[out]  dev         Initialized device descriptor of SM_PWM_01C device
 * @param[in]   params      The parameters for the SM_PWM_01C device
 *
 * @return                  SM_PWM_01C_OK on success
 * @return                  SM_PWM_01C_ERR_GPIO GPIO error
 */
int sm_pwm_01c_init(sm_pwm_01c_t* dev, const sm_pwm_01c_params_t* params);

/**
 * @brief       Start continuos measurement of Large and Small particle
 *              concentration
 *
 * @param[in]  dev         Device descriptor of SM_PWM_01C device
 */
void sm_pwm_01c_start(sm_pwm_01c_t* dev);

/**
 * @brief       Stops continuos measurement of Large and Small particle
 *              concentration
 *
 * @param[in]  dev         Device descriptor of SM_PWM_01C device
 */
void sm_pwm_01c_stop(sm_pwm_01c_t* dev);

/**
 * @brief       Reads concentration values for small particle  (1 ~ 2um)
 *
 * @param[in]  dev         Device descriptor of SM_PWM_01C device
 *
 * @returns                 Small particles concentraion in ug/m3
 */
int16_t sm_pwm_01c_read_tsp(sm_pwm_01c_t *dev);

/**
 * @brief       Reads concentration values for large particle (3 ~ 10um)
 *
 * @param[in]  dev         Device descriptor of SM_PWM_01C device
 *
 * @returns                 Large particles concentraion in ug/m3
 */
int16_t sm_pwm_01c_read_tlp(sm_pwm_01c_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SM_PWM_01C_PARAMS_H */
/** @} */
