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
 * @brief       uwb-core DPL (Decawave Porting Layer) mutex wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef MYNEWT_MUTEX_H
#define MYNEWT_MUTEX_H

#include "mynewt/types.h"
#include "mynewt/error.h"

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl mutex wrapper
 */
struct mynewt_mutex {
    mutex_t mutex;  /**< the mutex */
};

/**
 * @brief Initializes a mutex object.
 *
 * @param[out]  mu  pre-allocated mutex structure, must not be NULL.
 */
mynewt_error_t mynewt_mutex_init(struct mynewt_mutex *mu);

/**
 * @brief Pend (wait) for a mutex.
 *
 * @param[in]   mu      Pointer to mutex.
 * @param[in]   timeout Timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of OS_TIMEOUT_NEVER means wait forever.
 *
 * @return mynewt_error_t
 *      mynewt_INVALID_PARM    mutex passed in was NULL
 *      MYNEWT_OK              no error
 */
mynewt_error_t mynewt_mutex_pend(struct mynewt_mutex *mu, mynewt_time_t timeout);

/**
 *
 * @brief Release a mutex.
 *
 * @return mynewt_error_t
 *      mynewt_INVALID_PARM    mutex was NULL
 *      MYNEWT_OK              no error
 */
mynewt_error_t mynewt_mutex_release(struct mynewt_mutex *mu);

#ifdef __cplusplus
}
#endif

#endif /* MYNEWT_MUTEX_H */
