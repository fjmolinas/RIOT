/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_smtpe610
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef SMTPE610_PARAMS_H
#define SMTPE610_PARAMS_H

#include "board.h"
#include "smtpe610.h"
#include "smtpe610_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef SMTPE610_PARAM_PARAM1
#define SMTPE610_PARAM_PARAM1
#endif

#ifndef SMTPE610_PARAMS
#define SMTPE610_PARAMS
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const smtpe610_params_t smtpe610_params[] =
{
    SMTPE610_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* SMTPE610_PARAMS_H */
/** @} */
