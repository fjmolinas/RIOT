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

#ifndef OS_MUTEX_H
#define OS_MUTEX_H

#include "os/types.h"
#include "os/os_error.h"

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl mutex wrapper
 */
struct os_mutex {
    mutex_t mutex;  /**< the mutex */
};

/**
 * @brief Initializes a mutex object.
 *
 * @param[out]  mu  pre-allocated mutex structure, must not be NULL.
 */
os_error_t os_mutex_init(struct os_mutex *mu);

/**
 * @brief Pend (wait) for a mutex.
 *
 * @param[in]   mu      Pointer to mutex.
 * @param[in]   timeout Timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of OS_TIMEOUT_NEVER means wait forever.
 *
 * @return os_error_t
 *      os_INVALID_PARM    mutex passed in was NULL
 *      OS_OK              no error
 */
os_error_t os_mutex_pend(struct os_mutex *mu, os_time_t timeout);

/**
 *
 * @brief Release a mutex.
 *
 * @return os_error_t
 *      os_INVALID_PARM    mutex was NULL
 *      OS_OK              no error
 */
os_error_t os_mutex_release(struct os_mutex *mu);

#ifdef __cplusplus
}
#endif

#endif /* OS_MUTEX_H */
