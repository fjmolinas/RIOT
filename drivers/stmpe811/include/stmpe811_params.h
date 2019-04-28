/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stmpe811
 *
 * @{
 * @file
 * @brief       Default configuration for STMPE811
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef STMPE811_PARAMS_H
#define STMPE811_PARAMS_H

#include "board.h"
#include "stmpe811.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the STMPE811
 * @{
 */
#ifndef STMPE811_PARAM_I2C_DEV
#define STMPE811_PARAM_I2C_DEV          I2C_DEV(0)
#endif
#ifndef STMPE811_PARAM_ADDR
#define STMPE811_PARAM_ADDR             (0x41)
#endif

#ifndef STMPE811_PARAMS
#define STMPE811_PARAMS                { .i2c = STMPE811_PARAM_I2C_DEV, \
                                         .addr = STMPE811_PARAM_ADDR }
#endif
/**@}*/

/**
 * @brief   Configure STMPE811
 */
static const stmpe811_params_t stmpe811_params[] =
{
    STMPE811_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* STMPE811_PARAMS_H */
/** @} */
