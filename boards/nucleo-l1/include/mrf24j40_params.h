/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 *
 * @{
 * @file
 * @brief       Configuration for nucleo-l1 the MRF24J40 driver
 *
 */

#ifndef MRF24J40_PARAMS_H
#define MRF24J40_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MRF24J40_PARAM_SPI
#define MRF24J40_PARAM_SPI          (SPI_DEV(0))
#endif
#ifndef MRF24J40_PARAM_SPI_CLK
#define MRF24J40_PARAM_SPI_CLK      (SPI_CLK_5MHZ)
#endif
#ifndef MRF24J40_PARAM_CS
#define MRF24J40_PARAM_CS           (GPIO_PIN(PORT_B, 6))
#endif
#ifndef MRF24J40_PARAM_INT
#define MRF24J40_PARAM_INT          (GPIO_PIN(PORT_A, 9))
#endif
#ifndef MRF24J40_PARAM_RESET
#define MRF24J40_PARAM_RESET        (GPIO_PIN(PORT_A, 8))
#endif

#define MRF24J40_PARAMS_DEFAULT     { .spi = MRF24J40_PARAM_SPI, \
                                      .spi_clk = MRF24J40_PARAM_SPI_CLK, \
                                      .cs_pin = MRF24J40_PARAM_CS, \
                                      .int_pin = MRF24J40_PARAM_INT, \
                                      .reset_pin = MRF24J40_PARAM_RESET }

/**
 * @brief   MRF24J40 configuration
 */
static const mrf24j40_params_t mrf24j40_params[] =
{
#ifdef MRF24J40_PARAMS_BOARD
    MRF24J40_PARAMS_BOARD,
#else
    MRF24J40_PARAMS_DEFAULT,
#endif
};
#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_PARAMS_H */
/** @} */
