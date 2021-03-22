/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mics6814
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef MICS6814_PARAMS_H
#define MICS6814_PARAMS_H

#include "board.h"
#include "mics6814.h"
#include "mics6814_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef MICS6814_PARAM_OX_LINE
#define MICS6814_PARAM_OX_LINE       NRF52_AIN0 /* GPIO(0, 4) */
#endif
#ifndef MICS6814_PARAM_RED_LINE
#define MICS6814_PARAM_RED_LINE      NRF52_AIN1 /* GPIO(0, 5) */
#endif
#ifndef MICS6814_PARAM_NH3_LINE
#define MICS6814_PARAM_NH3_LINE      NRF52_AIN2 /* GPIO(0, 28) */
#endif
#ifndef MICS6814_PARAM_ADC_RES
#define MICS6814_PARAM_ADC_RES       ADC_RES_12BIT
#endif
#ifndef MICS6814_PARAMS
#define MICS6814_PARAMS             { .aox = MICS6814_PARAM_OX_LINE, \
                                      .ared = MICS6814_PARAM_RED_LINE, \
                                      .anh3 = MICS6814_PARAM_NH3_LINE, \
                                      .adc_res = MICS6814_PARAM_ADC_RES \
                                    }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const mics6814_params_t mics6814_params[] =
{
    MICS6814_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* MICS6814_PARAMS_H */
/** @} */
