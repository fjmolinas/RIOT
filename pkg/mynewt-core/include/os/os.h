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

#ifndef OS_OS_H
#define OS_OS_H

#include <assert.h>
#include <stdint.h>
#include <stdatomic.h>

#include "irq.h"
#include "os/types.h"

#if IS_USED(MODULE_NIMBLE)
#include "nimble/nimble_npl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a, b) ((a)>(b)?(a):(b))
#endif

#define OS_ALIGN(__n, __a) (                             \
        (((__n) & ((__a) - 1)) == 0)                   ? \
            (__n)                                      : \
            ((__n) + ((__a) - ((__n) & ((__a) - 1))))    \
        )

#define OS_ALIGNMENT    (4)

/**
 * @brief   CPU status register
 */
typedef uint32_t os_sr_t;

/**
 * @name    Entering and exiting critical section defines
 * @{
 */
#define OS_ENTER_CRITICAL(_sr) (_sr = os_hw_enter_critical())
#define OS_EXIT_CRITICAL(_sr) (os_hw_exit_critical(_sr))
#define OS_ASSERT_CRITICAL() assert(os_hw_is_in_critical())
/** @} */

/**
 * @brief   variable to check if ISR are disabled
 */
extern atomic_uint os_in_critical;

/**
 * @brief   Disable ISRs
 *
 * @return  current isr context
 */
static inline uint32_t os_hw_enter_critical(void)
{
    uint32_t ctx = irq_disable();
    unsigned int count = atomic_load(&os_in_critical);
    atomic_store(&os_in_critical, count + 1);
    return ctx;
}

/**
 * @brief   Restores ISR context
 *
 * @param[in]   ctx      ISR context to restore.
 */
static inline void os_hw_exit_critical(uint32_t ctx)
{
    unsigned int count = atomic_load(&os_in_critical);
    atomic_store(&os_in_critical, count - 1);
    irq_restore((unsigned)ctx);
}

/**
 * @brief Check if is in critical section
 *
 * @return true, if in critical section, false otherwise
 */
static inline bool os_hw_is_in_critical(void)
{
    /*
     * XXX Currently RIOT does not support an API for finding out if interrupts
     *     are currently disabled, hence in a critical section in this context.
     *     So for now, we use this global variable to keep this state for us.
     */
    return (atomic_load(&os_in_critical) > 0);
}

/* Mynewt components (not abstracted in NPL or DPL) */
#include "os/endian.h"
#include "os/queue.h"
#include "os/os_error.h"
#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "os/os_trace_api.h"

#ifdef __cplusplus
}
#endif

#endif /* OS_OS_H */
