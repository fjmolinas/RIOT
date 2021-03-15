/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ili9341 ILI9341 display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the ILI9341 display
 *
 * @{
 *
 * @file
 * @brief       Driver for ili941 display
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * The ILI9341 is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 */


#ifndef ILI9341_H
#define ILI9341_H

#include "lcd.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_ili9341_config     ILI9341 display driver compile configuration
 * @ingroup config_drivers_display
 * @{
 */
/**
 * @brief ILI9341 gvdd level.
 *
 * Default GVDD voltage of 4.8V. GVDD is reference level for the VCOM level and
 * the grayscale voltage level. GVDD should be ≦ (AVDD - 0.5) V .
 */
#ifndef CONFIG_ILI9341_GVDD
#define CONFIG_ILI9341_GVDD             4800
#endif

/**
 * @brief ILI9341 VCOMH voltage level.
 *
 * Default VCOMH voltage of 4.25V. VCOMH represents the high level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering.
 */
#ifndef CONFIG_ILI9341_VCOMH
#define CONFIG_ILI9341_VCOMH            4250
#endif

/**
 * @brief ILI9341 VCOML voltage level.
 *
 * Default VCOML voltage of -2V. VCOML represents the low level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering
 */
#ifndef CONFIG_ILI9341_VCOML
#define CONFIG_ILI9341_VCOML            -2000
#endif

/**
 * @brief   Device descriptor for a ili9341
 */
typedef struct {
    lcd_t dev;                    /**< Pointer to the common lcd device */
} ili9341_t;

/**
 * @brief   LCD device operations table
 */
extern const lcd_desc_t lcd_ili9341_driver;

#ifdef __cplusplus
}
#endif
#endif /* ILI9341_H */
/** @} */
