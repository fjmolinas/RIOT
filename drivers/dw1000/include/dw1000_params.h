/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dw1000
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef DW1000_PARAMS_H
#define DW1000_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef DW1000_PARAM_SPI
#define DW1000_PARAM_SPI        (SPI_DEV(1))
#endif
#ifndef DW1000_PARAM_SPI_CLK
#define DW1000_PARAM_SPI_CLK    (SPI_CLK_1MHZ)
#endif
#ifndef DW1000_SPI_MODE
#define DW1000_SPI_MODE         (SPI_MODE_0)
#endif
#ifndef DW1000_PARAM_CS
#define DW1000_PARAM_CS         (GPIO_PIN(0, 17))
#endif
#ifndef DW1000_PARAM_IRQ
#define DW1000_PARAM_IRQ        (GPIO_PIN(0, 19))
#endif
#ifndef DW1000_PARAM_RESET
#define DW1000_PARAM_RESET      (GPIO_PIN(0, 24))
#endif

#ifndef DW1000_PARAMS
#define DW1000_PARAMS           { .spi = DW1000_PARAM_SPI,         \
                                  .spi_clk = DW1000_PARAM_SPI_CLK, \
                                  .cs_pin = DW1000_PARAM_CS,       \
                                  .mode = DW1000_SPI_MODE,         \
                                  .irq_pin = DW1000_PARAM_IRQ,     \
                                  .reset_pin = DW1000_PARAM_RESET }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const dw1000_params_t dw1000_params[] =
{
    DW1000_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* DW1000_PARAMS_H */
/** @} */
