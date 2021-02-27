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

#ifndef OS_SEM_H
#define OS_SEM_H

#include <stdint.h>

#include "os/types.h"
#include "os/os_error.h"

#include "sema.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl semaphore wrapper
 */
struct os_sem {
    sema_t sema;    /**< the semaphore */
};

/**
 * @brief Initialize a semaphore
 *
 * @param[in]   sem     pointer to semaphore
 * @param[in]   tokens  # of tokens the semaphore should contain initially.
 *
 * @return os_error_t
 *      OS_INVALID_PARM     Semaphore passed in was NULL.
 *      OS_OK               no error.
 */
os_error_t os_sem_init(struct os_sem *sem, uint16_t tokens);

/**
 * @brief Pend (wait) for a semaphore.
 *
 * @param[in]   sem     pointer to semaphore.
 * @param[in]   timeout timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of OS_TIMEOUT_NEVER means wait forever.
 *
 *
 * @return os_error_t
 *      OS_INVALID_PARM     semaphore passed in was NULL.
 *      OS_TIMEOUT          semaphore was owned by another task and timeout=0
 *      OS_OK               no error
 */
os_error_t os_sem_pend(struct os_sem *sem, os_time_t timeout);

/**
 * @brief Release a semaphore.
 *
 * @param[in]   sem     pointer to the semaphore to be released
 *
 * @return os_error_t
 *      OS_INVALID_PARM    semaphore passed in was NULL.
 *      OS_OK              no error
 */
os_error_t os_sem_release(struct os_sem *sem);

/**
 * @brief Get current semaphore's count
 */
uint16_t os_sem_get_count(struct os_sem *sem);

#ifdef __cplusplus
}
#endif

#endif /* OS_SEM_H */
