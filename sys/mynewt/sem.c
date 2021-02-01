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
 * @brief       Decawave Porting Layer semaphore RIOT wrapper
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdio.h>

#include "irq.h"
#include "mynewt/sem.h"

mynewt_error_t mynewt_sem_init(struct mynewt_sem *sem, uint16_t tokens)
{
    if (!sem) {
        return mynewt_INVALID_PARAM;
    }

    sema_create(&sem->sema, tokens);
    return MYNEWT_OK;
}

mynewt_error_t mynewt_sem_release(struct mynewt_sem *sem)
{
    int ret;

    if (!sem) {
        return mynewt_INVALID_PARAM;
    }

    ret = sema_post(&sem->sema);

    return (ret) ? mynewt_ERROR : MYNEWT_OK;
}

uint16_t mynewt_sem_get_count(struct mynewt_sem *sem)
{
    unsigned state = irq_disable();
    unsigned int value = sem->sema.value;
    irq_restore(state);
    return value;
}

mynewt_error_t mynewt_sem_pend(struct mynewt_sem *sem, mynewt_time_t timeout)
{
    int ret = sema_wait_timed(&sem->sema, timeout);
    return (ret) ? mynewt_ERROR : MYNEWT_OK;
}
