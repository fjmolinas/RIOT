/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_im880b
 * @{
 *
 * @file
 * @brief       Board specific definitions for the im808b board.
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XTIMER_BACKOFF              (11)
#define XTIMER_OVERHEAD             (6)
#define XTIMER_WIDTH                (16)

/**
 * @name    sx1276 configuration
 * @{
 */
#define SX127X_PARAM_SPI_NSS                GPIO_PIN(PORT_B, 0)

#define SX127X_PARAM_RESET                  GPIO_PIN(PORT_A, 2)
#define SX127X_PARAM_DIO0                   GPIO_PIN(PORT_B, 1)
#define SX127X_PARAM_DIO1                   GPIO_PIN(PORT_B, 10)
#define SX127X_PARAM_DIO2                   GPIO_PIN(PORT_B, 11)
#define SX127X_PARAM_DIO3                   GPIO_PIN(PORT_B, 7)

#define SX127X_PARAM_RX_SWITCH              GPIO_PIN(PORT_C, 13)
#define SX127X_PARAM_TX_SWITCH              GPIO_PIN(PORT_A, 4)
/** @} */


/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
