/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_st7735
 *
 * @{
 * @file
 * @brief       Default configuration for st7735
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef ST7735_PARAMS_H
#define ST7735_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Default parameters for ST7735 display 
 *
 * @name    Set default configuration parameters for the ST7735
 * @{
 */
#ifndef ST7735_PARAM_SPI
#define ST7735_PARAM_SPI          SPI_DEV(1)
#endif
#ifndef ST7735_PARAM_SPI_CLK
#define ST7735_PARAM_SPI_CLK      SPI_CLK_5MHZ
#endif
#ifndef ST7735_PARAM_CS
#define ST7735_PARAM_CS           (GPIO_PIN(PA,14))
#endif
#ifndef ST7735_PARAM_DCX
#define ST7735_PARAM_DCX          (GPIO_PIN(PA,15))
#endif
#ifndef ST7735_PARAM_RST
#define ST7735_PARAM_RST          (GPIO_PIN(PA,8))
#endif

#if defined(ST7735_128x128)
#define ST7735_WIDTH              (0x7f)
#define ST7735_HEIGHT             (0x7f)
#define ST7735_OFFSET_X           (2U)
#define ST7735_OFFSET_Y           (3U)
#elif defined(ST7735_128x160)
#define ST7735_WIDTH              (0x7f)
#define ST7735_HEIGHT             (0x9f)
#define ST7735_OFFSET_X           (1U)
#define ST7735_OFFSET_Y           (2U)
#elif defined(ST7735_80x160)
#define ST7735_WIDTH              (0x7f)
#define ST7735_HEIGHT             (0x4F)
#define ST7735_OFFSET_X           (24U)
#define ST7735_OFFSET_Y           (0U)
#else
#error "Define ST7735_128x128, ST7735_128x160 or ST7735_80x160"
#endif

#ifndef ST7735_PARAMS
#define ST7735_PARAMS              { .spi = ST7735_PARAM_SPI, \
                                     .spi_clk = ST7735_PARAM_SPI_CLK, \
                                     .cs_pin = ST7735_PARAM_CS, \
                                     .dcx_pin = ST7735_PARAM_DCX, \
                                     .rst_pin = ST7735_PARAM_RST }
#endif
/**@}*/

/**
 * @brief   Configure ST7735
 */
static const st7735_params_t st7735_params[] =
{
    ST7735_PARAMS,
};

#ifdef __cplusplus
}
#endif

#endif /* ST7735_PARAMS_H */