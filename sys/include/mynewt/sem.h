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
 * @brief       uwb-core DPL (Decawave Porting Layer) semapahore wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef MYNEWT_SEM_H
#define MYNEWT_SEM_H

#include <stdint.h>

#include "mynewt_types.h"
#include "mynewt_error.h"

#include "sema.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl semaphore wrapper
 */
struct mynewt_sem {
    sema_t sema;    /**< the semaphore */
};

/**
 * @brief Initialize a semaphore
 *
 * @param[in]   sem     pointer to semaphore
 * @param[in]   tokens  # of tokens the semaphore should contain initially.
 *
 * @return mynewt_error_t
 *      MYNEWT_INVALID_PARM     Semaphore passed in was NULL.
 *      MYNEWT_OK               no error.
 */
mynewt_error_t mynewt_sem_init(struct mynewt_sem *sem, uint16_t tokens);

/**
 * @brief Pend (wait) for a semaphore.
 *
 * @param[in]   sem     pointer to semaphore.
 * @param[in]   timeout timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of MYNEWT_TIMEOUT_NEVER means wait forever.
 *
 *
 * @return mynewt_error_t
 *      MYNEWT_INVALID_PARM     semaphore passed in was NULL.
 *      MYNEWT_TIMEOUT          semaphore was owned by another task and timeout=0
 *      MYNEWT_OK               no error
 */
mynewt_error_t mynewt_sem_pend(struct mynewt_sem *sem, mynewt_time_t timeout);

/**
 * @brief Release a semaphore.
 *
 * @param[in]   sem     pointer to the semaphore to be released
 *
 * @return mynewt_error_t
 *      MYNEWT_INVALID_PARM    semaphore passed in was NULL.
 *      MYNEWT_OK              no error
 */
mynewt_error_t mynewt_sem_release(struct mynewt_sem *sem);

/**
 * @brief Get current semaphore's count
 */
uint16_t mynewt_sem_get_count(struct mynewt_sem *sem);

#ifdef __cplusplus
}
#endif

#endif /* MYNEWT_SEM_H */
