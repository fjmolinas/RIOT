#ifndef TMP102_H
#define TMP102_H

#include <stdint.h>
#include "periph/i2c.h"
/**
 * @brief   TM102 Default Address, A0 conencted to GND
 */
#ifndef TMP102_I2C_ADDRESS
#define TMP102_I2C_ADDRESS         (0x48)
#endif

/**
 * @brief   Default Conversion Time in us
 */
#ifndef TMP102_CONVERSION_TIME
#define TMP102_CONVERSION_TIME     (1E6)
#endif

/**
 * @brief   Default low power mode
 *
 * If set to 0, the device will be always-on
 * If set to 1, the device will be put in sleep mode bewtween sleps.
 * This adds a @c TMP102_CONVERSION_TIME us delay to each measurement call
 * for bringing the device out of standby.
 */
#ifndef TMP102_USE_LOW_POWER
#define TMP102_USE_LOW_POWER (0)
#endif

/**
 * @brief   Default raw value mode
 *
 * If set to 0, measurements will be converted to Celsius.
 * If set to 1, raw adc readings will be returned.
 */
#ifndef TMP102_USE_RAW_VALUES
#define TMP102_USE_RAW_VALUES (0)
#endif

/**
 * @name    Conversion rate and AVG sampling configuration
 * @{
 */
#define TMP102_CONFIG_CR0       (0x00)   /**< Conversion Time 0.25s*/
#define TMP102_CONFIG_CR1       (0x01)   /**< Conversion Time 1s*/
#define TMP102_CONFIG_CR2       (0x02)   /**< Conversion Time 4s*/
#define TMP102_CONFIG_CR3       (0x03)   /**< Conversion Time 8s,*/
#define TMP102_CONFIG_CR_DEF    TMP102_CONFIG_CR2 /**< Default for Testing */
/** @} */

/**
 * @brief   TMP102 specific return values
 */
enum {
    TMP102_OK,          /**< Success, no error */
    TMP102_ERROR_BUS,   /**< I2C bus error */
    TMP102_ERROR_DEV,   /**< internal device error */
    TMP102_ERROR_CONF,  /**< invalid device configuration */
    TMP102_ERROR,       /**< general error */
};

/**
 * @brief   TMP102 specific return values
 */
enum {
    TMP102_12BIT,
    TMP102_13BIT,
};

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    i2c_t   i2c;        /**< I2C device, the sensor is connected to */
    uint8_t addr;       /**< the sensor's slave address on the I2C bus */
    uint8_t rate;       /**< conversion rate */
} tmp102_params_t;

/**
 * @brief   Device descriptor for TMP102 sensors.
 */
typedef struct {
    tmp102_params_t params;  /**< Configuration parameters */
} tmp102_t;

/**
 * @brief   Initialize the TMP102 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  params       configuration parameters
 *
 * @return                  0 on success
 * @return                  -TMP102_ERROR_BUS on I2C bus error
 * @return                  -TMP102_ERROR_DEV if sensor test failed
 * @return                  -TMP102_ERROR_CONF if sensor configuration failed
 */
int tmp102_init(tmp102_t *dev, const tmp102_params_t *params);

/**
 * @brief   Initialize the TMP102 sensor driver.
 *
 * @param[out] dev          device descriptor of sensor to initialize
 * @param[in]  mode         TMP102_12BIT or TMP102_13BIT
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp102_set_extended_mode(tmp102_t *dev, uint8_t mode);

/**
 * @brief   Set sleep mode.
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp102_sleep(tmp102_t *dev);

/**
 * @brief   Wake up device
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp102_wakeup(tmp102_t *dev);

/**
 * @brief   Read sensor's data.
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] rawt         raw die temperature
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int tmp102_read(const tmp102_t *dev, int16_t *rawt);

/**
 * @brief   Convert raw sensor values to temperature.
 *
 * @param[in]  rawt         raw temperature value
 * @param[out] tamb         converted ambient temperature
 */
void tmp102_convert(int16_t rawt, float *tamb);

/**
 * @brief   Convenience function to get ambient temperatures in [°C]
 *
 * @note    Temperature scaled by x100 for accuracy and avoid floats
 *
 * @param[in]  dev          device descriptor of sensor
 * @param[out] ta           converted ambient temperature
 */
int tmp102_read_temperature(const tmp102_t *dev, int16_t *ta);

#endif /* TMP102_H*/
