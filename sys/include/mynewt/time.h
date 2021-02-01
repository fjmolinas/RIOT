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
 * @brief       uwb-core DPL (Decawave Porting Layer) time abstraction
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef MYNEWT_TIME_H
#define MYNEWT_TIME_H

#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DPL ticks per seconds
 */
#define MYNEWT_TICKS_PER_SEC (XTIMER_HZ)

/**
 * @brief Returns the low 32 bits of cputime.
 *
 * @return uint32_t The lower 32 bits of cputime
 */
static inline mynewt_time_t mynewt_time_get(void)
{
    return xtimer_now().ticks32;
}

/**
 * @brief Converts the given number of milliseconds into cputime ticks.
 *
 * @param[in]   ms          The number of milliseconds to convert to ticks
 * @param[out]  out_ticks   The number of ticks corresponding to 'ms'
 *
 * @return mynewt_error_t  MYNEWT_OK - no error
 */
static inline mynewt_error_t mynewt_time_ms_to_ticks(uint32_t ms, mynewt_time_t *out_ticks)
{
    *out_ticks = xtimer_ticks_from_usec(ms * US_PER_MS).ticks32;
    return MYNEWT_OK;
}

/**
 * @brief Convert the given number of ticks into milliseconds.
 *
 * @param[in]   ticks   The number of ticks to convert to milliseconds.
 * @param[out]  out_ms  The converted milliseconds from 'ticks'
 *
 * @return mynewt_error_t  MYNEWT_OK - no error
 */
static inline mynewt_error_t  mynewt_time_ticks_to_ms(mynewt_time_t ticks, uint32_t *out_ms)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    *out_ms = xtimer_usec_from_ticks(val) * US_PER_MS;
    return MYNEWT_OK;
}

/**
 * @brief   Converts the given number of milliseconds into cputime ticks.
 *
 * @param[in]   ms  The number of milliseconds to convert to ticks
 *
 * @return  uint32_t    The number of ticks corresponding to 'ms'
 */
static inline mynewt_time_t mynewt_time_ms_to_ticks32(uint32_t ms)
{
    return xtimer_ticks_from_usec(ms * US_PER_MS).ticks32;
}

/**
 * @brief   Convert the given number of ticks into milliseconds.
 *
 * @param[in]   ticks   The number of ticks to convert to milliseconds.
 *
 * @return  uint32_t    The number of milliseconds corresponding to 'ticks'
 */
static inline mynewt_time_t mynewt_time_ticks_to_ms32(mynewt_time_t ticks)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    return xtimer_usec_from_ticks(val) * US_PER_MS;
}

/**
 * @brief   Wait until the number of ticks has elapsed, BLOICKING.
 *
 * @param[in]   ticks   The number of ticks to wait.
 */
static inline void mynewt_time_delay(mynewt_time_t ticks)
{
    xtimer_ticks32_t val = {.ticks32 = ticks};
    xtimer_tsleep32((xtimer_ticks32_t) val);
}

#ifdef __cplusplus
}
#endif

#endif /* MYNEWT_TIME_H */
