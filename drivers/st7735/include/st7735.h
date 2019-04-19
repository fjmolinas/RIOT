/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_st7735 st7735 display driver
 * @ingroup     drivers
 *
 * @brief       st7735 Display driver
 *
 * @{
 *
 * @file
 * @brief       Driver for st7735 display
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *              Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * The ST7735 is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 * The driver requires colors to be send in big endian RGB-565 format.
 */

#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;          /**< SPI device that the display is connected to */
    spi_clk_t spi_clk;  /**< SPI clock speed to use */
    gpio_t cs_pin;      /**< pin connected to the CHIP SELECT line */
    gpio_t dcx_pin;      /**< pin connected to the CD line */
    gpio_t rst_pin;     /**< pin connected to the reset line */
} st7735_params_t;

/**
 * @brief   Device descriptor for a st7735
 */
typedef struct {
    st7735_params_t params;          /**< I2C device that sensor is connected to */
} st7735_t;

/**
 * @brief   Setup an st7735 display device
 *
 * @param[out]  dev     device descriptor
 * @param[in]   params  parameters for device initialization
 */
int st7735_init(st7735_t *dev, const st7735_params_t *prms);

/**
 * @brief   Raw read command
 *
 * @pre         len > 0
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[out]  data    data from the device
 * @param[in]   len     length of the returned data
 */
void st7735_read_cmd(st7735_t *dev, uint8_t cmd, uint8_t *data, size_t len);

/**
 * @brief   Fill a rectangular area with a single color
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   single color to fill the area with
 */
void st7735_fill(st7735_t *dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint16_t color);

/**
 * @brief   Fill a rectangular area with an array of colors
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   array of colors to fill the area with
 */
void st7735_map(st7735_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                 uint16_t y2, const uint16_t *color);

#ifdef __cplusplus
}
#endif