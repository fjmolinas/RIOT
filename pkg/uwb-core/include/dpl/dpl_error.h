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
 * @brief DPL error types
 */
enum dpl_error {
    DPL_OK  = MYNEWT_OK,
    DPL_ENOMEM = MYNEWT_ENOMEM,
    DPL_EINVAL = MYNEWT_EINVAL,
    DPL_INVALID_PARAM = MYNEWT_INVALID_PARAM,
    DPL_MEM_NOT_ALIGNED = MYNEWT_MEM_NOT_ALIGNED,
    DPL_BAD_MUTEX = MYNEWT_BAD_MUTEX,
    DPL_TIMEOUT = MYNEWT_TIMEOUT,
    DPL_ERR_IN_ISR = MYNEWT_ERR_IN_ISR,
    DPL_ERR_PRIV = MYNEWT_ERR_PRIV,
    DPL_OS_NOT_STARTED = MYNEWT_OS_NOT_STARTED,
    DPL_ENOENT = MYNEWT_ENOENT,
    DPL_EBUSY = MYNEWT_EBUSY,
    DPL_ERROR = MYNEWT_ERROR ,
};


/**
 * @brief dpl error type
 */
typedef enum dpl_error dpl_error_t;

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_ERROR_H */
