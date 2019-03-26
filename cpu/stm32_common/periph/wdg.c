/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_wdg
 *
 * @brief
 *
 * @{
 *
 * @file        wdg.c
 * @brief       Independent Watchdog timer for STM32L platforms
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "assert.h"
#include "cpu.h"
#include "periph_cpu_common.h"
#include "periph/wdg.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RELOAD                (4096U)
#define MAX_PRESCALER             (6U)

#define LSI_CLOCK_US              (US_PER_SEC / CLOCK_LSI)
#define IWDG_MIN_US               ((4U) * (LSI_CLOCK_US))
#define IWDG_STEP_US              ((4U) * (LSI_CLOCK_US)*(MAX_RELOAD))
#define IWDG_MAX_US               ((4U) * (LSI_CLOCK_US)*(MAX_RELOAD) * \
                                   (1 << MAX_PRESCALER))

#define IWDG_KR_KEY_RELOAD        ((uint16_t)0xAAAA)
#define IWDG_KR_KEY_ENABLE        ((uint16_t)0xCCCC)

#define IWDG_UNLOCK               ((uint16_t)0x5555)
#define IWDG_LOCK                 ((uint16_t)0x0000)

#if ENABLE_DEBUG
/* wdg_time (us) = LSI(us) x 4 x 2^PRE x RELOAD */
static inline uint32_t _wdg_time(uint8_t pre, uint16_t rel)
{
    return ((rel * (1 << pre) * LSI_CLOCK_US * 4));
}
#endif

static inline void _iwdg_unlock(void)
{
    IWDG->KR = IWDG_UNLOCK;
}

static inline void _iwdg_lock(void)
{
    IWDG->KR = IWDG_LOCK;
}

static void _set_prescaler(uint8_t prescaler)
{
    assert(prescaler <= MAX_PRESCALER);

    _iwdg_unlock();
    IWDG->PR = prescaler;
    _iwdg_lock();
}

static void _set_reload(uint16_t reload)
{
    assert(reload <= IWDG_RLR_RL);

    _iwdg_unlock();
    IWDG->RLR = reload;
    _iwdg_lock();
}

static uint8_t _find_prescaler(uint32_t rst_time)
{
    /* Divide by the range to get power of 2 of the prescaler*/
    uint16_t r = rst_time / IWDG_STEP_US;
    uint8_t pre = 0;

    DEBUG("[wdg]: range value %d\n", (int)r);
    /* Calculate prescaler */
    while (r > 0) {
        r = r >> 1;
        pre++;
    }
    DEBUG("[wdg]: prescaler value %d\n", pre);
    return pre;
}

static uint16_t _find_reload_value(uint8_t pre, uint32_t rst_time)
{
    /* Calculate best reload value*/
    uint16_t rel = (uint16_t)(rst_time / \
                              ((uint32_t)(LSI_CLOCK_US * 4 * (1 << pre))));

    DEBUG("[wdg]: reload value %d\n", rel);
    return rel;
}

void wdg_enable(void)
{
    IWDG->KR = IWDG_KR_KEY_ENABLE;
}

void wdg_disable(void)
{
#ifdef CPU_FAM_STM32L4
    DEBUG("[Warning]: wdg will freeze in STOP and STANDBY mode");
    FLASH->OPTR |= ~(FLASH_OPTR_IWDG_STOP || FLASH_OPTR_IWDG_STDBY);
#else
    DEBUG("[Warning]: wdg can't be disabled on stm32l platforms once \
           enabled,only by a reset");
#endif
}

void wdg_reset(void)
{a
    IWDG->KR = IWDG_KR_KEY_RELOAD;
}

int wdg_init(uint32_t rst_time)
{
    /* Check reset time limit */
    if ((rst_time < IWDG_MIN_US) || (rst_time > IWDG_MAX_US)) {
        DEBUG("[wdg]: out of bounds values [us]\n");
        return WDG_ERROR;
    }

    uint8_t pre = _find_prescaler(rst_time);
    uint16_t rel = _find_reload_value(pre, rst_time);

    /* Set watchdog prescaler and reload value */
    _set_prescaler(pre);
    _set_reload(rel);

#if ENABLE_DEBUG
    DEBUG("[wdg]: reset time %lu [us]\n", _wdg_time(pre, rel));
#endif

    /* Refresh wdg counter*/
    wdg_reset();

    return WDG_OK;
}

uint32_t wdg_min_timeout(void)
{
    return IWDG_MIN_US;
}

uint32_t wdg_max_timeout(void)
{
    return IWDG_MAX_US;
}

#ifdef __cplusplus
}
#endif
