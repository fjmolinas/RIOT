
#include "opentimers.h"
#include "scheduler.h"
#include "leds.h"

#include "memarray.h"
#include "periph/pm.h"
#include "ztimer.h"
#include "ztimer/convert.h"
#include "ztimer/convert_frac.h"
#include "ztimer/convert_shift.h"
#include "ztimer/config.h"
#include "ztimer/periodic.h"

#include "periph_conf.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

/**
 * @brief   Maximum counter difference to not consider an ISR late, this
 *          should account for the largest timer interval OpenWSN
 *          scheduler might work with. When running only the stack this
 *          should not be more than SLOT_DURATION, but when using cjoin
 *          it is 65535ms
 */
#ifndef LOOP_THRESHOLD
#define LOOP_THRESHOLD       (2 * PORT_TICS_PER_MS * 65535)
#endif

#if CONFIG_ZTIMER_MSEC_BASE_FREQ > 32768U
#error No suitable ZTIMER_MSEC_BASE config. Maybe add USEMODULE += ztimer_usec?
static ztimer_convert_frac_t _ztimer_convert_frac_32768;
#define ZTIMER_32768_CONVERT_LOWER_FREQ    CONFIG_ZTIMER_MSEC_BASE_FREQ
#define ZTIMER_32768_CONVERT_LOWER         (ZTIMER_MSEC_BASE)
/* cppcheck-suppress preprocessorErrorDirective
 * (reason: cppcheck fails to see that CONFIG_ZTIMER_MSEC_BASE_FREQ
 * is set in ztimer/config.h to a non zero value */
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
    ((32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ) == 0)
#error No suitable ZTIMER_MSEC_BASE config. Maybe add USEMODULE += ztimer_usec?
static ztimer_convert_shift_t _ztimer_convert_shift_32768;
#define ZTIMER_32768_CONVERT_HIGHER_FREQ   CONFIG_ZTIMER_MSEC_BASE_FREQ
#define ZTIMER_32768_CONVERT_HIGHER        (ZTIMER_MSEC_BASE)
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
    ((32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ) != 0)
#error No suitable ZTIMER_MSEC_BASE config. Maybe add USEMODULE += ztimer_usec?
#endif

static opentimers_vars_t _opentimers_vars;
static ztimer_clock_t *ZTIMER_32768 = NULL;

void _opentimers_timer_callback(void* arg)
{
    _opentimers_vars.current_compare_value = ztimer_now(ZTIMER_32768);

    opentimers_t * timer = (opentimers_t *) arg;

    if (timer->isr) {
        timer->cb(NULL);
    }
    else {
        scheduler_push_task((task_cbt) timer->cb, timer->prio);
    }
}

int _opentimers_timer_callback_periodic(void* arg)
{
    _opentimers_timer_callback(arg);
    /* allways keep going unless stoped explicitly */
    return false;
}

void opentimers_init(void)
{
    memset(&_opentimers_vars, 0, sizeof(opentimers_vars_t));
    memarray_init(&_opentimers_vars.memarray, &_opentimers_vars.timer_buff,
                  sizeof(opentimers_t), MAX_NUM_TIMERS);

#if CONFIG_ZTIMER_MSEC_BASE_FREQ > 32768U
    ZTIMER_32768 = &_ztimer_convert_frac_32768.super.super;
/* cppcheck-suppress preprocessorErrorDirective
 * (reason: cppcheck fails to see that CONFIG_ZTIMER_MSEC_BASE_FREQ
 * is set in ztimer/config.h to a non zero value */
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
    (32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ == 0)
    ZTIMER_32768 = &_ztimer_convert_shift_32768.super.super;
#elif CONFIG_ZTIMER_MSEC_BASE_FREQ == 32768U
    ZTIMER_32768 = ZTIMER_MSEC_BASE;
#else
#error Invalid ZTIMER_MSEC_BASE_FREQ config. Maybe add USEMODULE += ztimer_usec?
#endif

#if defined(ZTIMER_32768_CONVERT_LOWER)
    LOG_DEBUG("[sctimer]: ZTIMER_32768 convert_frac from %lu to 32768\n",
              (long unsigned)ZTIMER_32768_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_32768,
                             ZTIMER_32768_CONVERT_LOWER,
                             SCTIMER_FREQUENCY,
                             ZTIMER_32768_CONVERT_LOWER_FREQ);
#elif defined(ZTIMER_32768_CONVERT_HIGHER)
    LOG_DEBUG("[sctimer]: ZTIMER_32768 convert_shift %lu to 32768\n",
              (long unsigned)ZTIMER_32768_CONVERT_HIGHER_FREQ);
    ztimer_convert_shift_up_init(&_ztimer_convert_shift_32768,
                                 ZTIMER_32768_CONVERT_HIGHER,
                                 __builtin_ctz(SCTIMER_FREQUENCY /
                                               CONFIG_ZTIMER_MSEC_BASE_FREQ));
#endif
}

opentimers_id_t opentimers_create(uint8_t timer_id, uint8_t task_prio)
{
    opentimers_t* timer = memarray_calloc(&_opentimers_vars.memarray);

    if (!timer) {
        /* timer list has overflown. This should never happen! */
        LOG_ERROR("[drivers/opentimers]: no available timers\n");
        leds_error_blink();
        pm_off();
    }

    if (timer_id > 1) {
        timer->isr = false;
    }
    else {
        timer->isr = true;
    }

    timer->prio = task_prio;

    return timer;
}

void opentimers_scheduleIn(opentimers_id_t id, uint32_t duration,
                           time_type_t uint_type, timer_type_t timer_type,
                           opentimers_cbt cb)
{
    opentimers_t *opentimer = id;
    opentimer->type = timer_type;
    opentimer->unit = uint_type;
    opentimer->cb = cb;

    /* use correct clock */
    ztimer_clock_t *clock;
    if (uint_type == TIME_MS) {
        clock = ZTIMER_MSEC;
    }
    else {
        clock = ZTIMER_32768;
    }
    /* start or setup timer */
    if (timer_type == TIMER_ONESHOT) {
        ztimer_t* timer =  &opentimer->timer.timer;
        timer->arg = opentimer;
        timer->callback = _opentimers_timer_callback;
        ztimer_set(clock, timer, duration);
    } else {
        ztimer_periodic_t *timer = &opentimer->timer;
        ztimer_periodic_init(clock, timer, _opentimers_timer_callback_periodic,
                             opentimer, duration);
        ztimer_periodic_start(timer);
    }

}

void opentimers_scheduleAbsolute(opentimers_id_t id, uint32_t duration,
                                 PORT_TIMER_WIDTH reference,
                                 time_type_t uint_type, opentimers_cbt cb)
{
    opentimers_t *opentimer = id;
    opentimer->type = TIMER_ONESHOT;
    opentimer->unit = uint_type;
    opentimer->cb = cb;

    /* use correct clock */
    ztimer_clock_t *clock;
    if (uint_type == TIME_MS) {
        clock = ZTIMER_MSEC;
    }
    else {
        clock = ZTIMER_32768;
    }

    /* arm timer */
    ztimer_t* timer = &opentimer->timer.timer;
    timer->arg = opentimer;
    timer->callback = _opentimers_timer_callback;
    unsigned state = irq_disable();
    uint32_t now = ztimer_now(clock);
    uint32_t target = duration + reference;

    /* if the next compare value (isr) to schedule is already later than
       the required value, but close enough to think we have been slow
       in scheduling it, trigger the ISR right away */
    if (now > target) {
        if (now - target < LOOP_THRESHOLD) {
            ztimer_set(clock, timer, 0);
        }
        else {
            ztimer_set(clock, timer,
                       UINT32_MAX - now + target);
        }
    }
    else {
        ztimer_set(clock, timer, target - now);
    }

    irq_restore(state);
}


void opentimers_updateDuration(opentimers_id_t id, PORT_TIMER_WIDTH duration)
{
    opentimers_t *opentimer = id;
    /* set correct unit */
    ztimer_clock_t *clock;
    if (opentimer->unit == TIME_MS) {
        clock = ZTIMER_MSEC;
    }
    else {
        clock = ZTIMER_32768;
    }
    if (opentimer->type == TIMER_ONESHOT) {
        ztimer_t* timer = &opentimer->timer.timer;
        timer->callback = _opentimers_timer_callback;
        ztimer_set(clock, timer, duration);
    } else {
        ztimer_periodic_t *timer = &opentimer->timer;
        ztimer_periodic_init(clock, timer, _opentimers_timer_callback_periodic,
                             opentimer, duration);
        ztimer_periodic_start(timer);
    }
}

void opentimers_cancel(opentimers_id_t id)
{
    opentimers_t *opentimer = id;
    ztimer_clock_t *clock;
    if (opentimer->unit == TIME_MS) {
        clock = ZTIMER_MSEC;
    }
    else {
        clock = ZTIMER_32768;
    }
    if (opentimer->type == TIMER_ONESHOT) {
        ztimer_t* timer = &opentimer->timer.timer;
        ztimer_remove(clock, timer);
    } else {
        ztimer_periodic_t *timer = &opentimer->timer;
        ztimer_periodic_stop(timer);
    }
}

bool opentimers_destroy(opentimers_id_t id)
{
    opentimers_cancel(id);
    memarray_free(&_opentimers_vars.memarray, id);
    return true;
}

PORT_TIMER_WIDTH opentimers_getValue(void)
{
    return ztimer_now(ZTIMER_32768);
}

PORT_TIMER_WIDTH opentimers_getCurrentCompareValue(void)
{
    return _opentimers_vars.current_compare_value;
}

