/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       Provides an adaption of OpenWSN led handling
 *              to RIOTs handling of LEDs and/or GPIOs
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#ifndef __LEDPINS_RIOT_H
#define __LEDPINS_RIOT_H


#include "board.h"
#include "periph/gpio.h"

/**
 * Holds a configuration of LED pins for debugging OpenWSN
 */
typedef struct ledpins_config {
    gpio_t error;
    gpio_t sync;
    gpio_t radio;
    gpio_t debug;
    uint8_t led_on;
} ledpins_config_t;

/**
 * @brief   Led on state values
 */
enum {
    GPIO_LED_LOW = 0,
    GPIO_LED_HIGH
};

/*
 * Set default configuration parameters for ledpins. Default settings match
 * OpenWSN openmote-b configuration.
 *
 * On Nucleo boards the LED pin is shared with SPI -> don't use it!
 */
#if defined (LED0_PIN) && !defined(MODULE_BOARDS_COMMON_NUCLEO)
#define OPENWSN_LEDPIN_ERROR            LED0_PIN
#else
#define OPENWSN_LEDPIN_ERROR            GPIO_UNDEF
#endif

#ifdef LED1_PIN
#define OPENWSN_LEDPIN_SYNC             LED1_PIN
#else
#define OPENWSN_LEDPIN_SYNC             GPIO_UNDEF
#endif

#ifdef LED3_PIN
#define OPENWSN_LEDPIN_RADIO            LED3_PIN
#else
#define OPENWSN_LEDPIN_RADIO            GPIO_UNDEF
#endif

#ifdef LED2_PIN
#define OPENWSN_LEDPIN_DEBUG            LED2_PIN
#else
#define OPENWSN_LEDPIN_DEBUG            GPIO_UNDEF
#endif

#ifdef BOARD_OPENMOTE_B
#define OPENWSN_LED_ON_STATE             GPIO_LED_LOW
#else
#define OPENWSN_LED_ON_STATE             GPIO_LED_HIGH
#endif

#define OPENWSN_LEDPINS_DEFAULT    { .error = OPENWSN_LEDPIN_ERROR, \
                                     .sync = OPENWSN_LEDPIN_SYNC, \
                                     .radio = OPENWSN_LEDPIN_RADIO, \
                                     .debug = OPENWSN_LEDPIN_DEBUG, \
                                     .led_on = OPENWSN_LED_ON_STATE }


static const ledpins_config_t openwsn_ledpins_params[] =
{
#ifdef OPENWSN_LEDPINS_BOARD
    OPENWSN_LEDPINS_BOARD,
#else
    OPENWSN_LEDPINS_DEFAULT,
#endif
};

void ledpins_riot_init(const ledpins_config_t *user_config);

#endif /* __LEDPINS_RIOT_H */
