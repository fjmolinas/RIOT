/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM R21 Xplained Pro
 *              board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(1)
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_USEC_TYPE    ZTIMER_TYPE_PERIPH_TIMER
#define CONFIG_ZTIMER_USEC_DEV     TIMER_DEV(1)
/* timer_set() may underflow for values smaller than 9, set 10 as margin */
#define CONFIG_ZTIMER_USEC_MIN     (10)
/** @} */

/**
 * @name    AT86RF233 configuration
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PB, 15)

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 19)

#define LED_PORT            PORT->Group[0]
#define LED0_MASK           (1 << 19)

#define LED0_ON             (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name    SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           PORT->Group[0]
#define BTN0_PIN            GPIO_PIN(0, 28)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Antenna configuration pin interface
 * @{
 */
#define RFCTL1_PIN          GPIO_PIN(0, 9)
#define RFCTL2_PIN          GPIO_PIN(0, 12)
/** @} */

/**
 * @brief   Antenna configuration values
 */
enum {
    RFCTL_ANTENNA_BOARD,
    RFCTL_ANTENNA_EXT,
};

/**
 * @name    Default antenna configuration
 * @{
 */
#ifndef RFCTL_ANTENNA_DEFAULT
#define RFCTL_ANTENNA_DEFAULT      RFCTL_ANTENNA_BOARD
#endif
/** @} */


/**
 * @name    Set default configuration parameters for the sdcard_spi driver
 * @{
 */
#ifndef SDCARD_SPI_PARAM_SPI
#define SDCARD_SPI_PARAM_SPI         (SPI_DEV(1))
#endif
#ifndef SDCARD_SPI_PARAM_CS
#define SDCARD_SPI_PARAM_CS          (GPIO_PIN(PB,3))
#endif
#ifndef SDCARD_SPI_PARAM_CLK
#define SDCARD_SPI_PARAM_CLK         (GPIO_PIN(PB,23))
#endif
#ifndef SDCARD_SPI_PARAM_MOSI
#define SDCARD_SPI_PARAM_MOSI        (GPIO_PIN(PB,22))
#endif
#ifndef SDCARD_SPI_PARAM_MISO
#define SDCARD_SPI_PARAM_MISO        (GPIO_PIN(PB,2))
#endif
#ifndef SDCARD_SPI_PARAM_POWER
#define SDCARD_SPI_PARAM_POWER       (GPIO_UNDEF)
#endif
#ifndef SDCARD_SPI_PARAM_POWER_AH
#define SDCARD_SPI_PARAM_POWER_AH    (true)
#endif

/**
 * @name    Set default configuration parameters for the MCP2515
 * @{
 */
#define MCP2515_PARAM_SPI SPI_DEV(1)
#define MCP2515_PARAM_CS GPIO_PIN(PA, 14)
#define MCP2515_PARAM_INT GPIO_PIN(PA, 15)

#if defined(MODULE_MTD_SDCARD) || defined(DOXYGEN)
#include "mtd_sdcard.h"
/**
 * @brief MTD device 0 (SD Card) definition. mtd0 is defined in board.c
 * @{
 */
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
#endif
/** @} */

/**
 * @brief   Set antenna switch
 */
void board_antenna_config(uint8_t antenna);
#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
