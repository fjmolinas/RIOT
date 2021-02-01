/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) error types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_ERROR_H
#define DPL_DPL_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mynewt/error.h"

/**
 * @brief dpl error type
 */
typedef mynewt_error_t dpl_error_t;

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_ERROR_H */
