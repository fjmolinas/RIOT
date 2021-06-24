/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for ztimer_adjust_time functionality
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "assert.h"
#include "timex.h"
#include "ztimer.h"
#include "mutex.h"
#include "atomic_utils.h"
#include "embUnit.h"

#define ENABLE_DEBUG    1
#include "debug.h"

#define     MARGIN          (US_PER_SEC / 2)

typedef struct {
    ztimer_t timer;
    uint32_t scheduled;
    uint32_t timeout;
    mutex_t lock;
} record_t;

static void setUp(void)
{
    /* Initialize */
    ZTIMER_EPOCH->adjust_time = 0;
}

static void tearDown(void)
{
    /* Finalize */
}

static void callback(void *arg)
{
    record_t *record = arg;

    mutex_unlock(&record->lock);
    atomic_store_u32(&record->timeout, ztimer_now(ZTIMER_USEC));
}

static bool is_in_range(uint32_t time, uint32_t target)
{
    if (time < target + MARGIN && time > (int64_t)target - MARGIN) {
        return true;
    }
    return false;
}

static void test_adjust_time_back(void)
{
    record_t record = { .timer = { .callback = callback, .arg = &record },
                        .lock = MUTEX_INIT_LOCKED,
                        .timeout = UINT32_MAX, };

    const uint8_t timeout = 2;
    const int8_t correction = -1;

    DEBUG("time %" PRIu32 " ms: set record in %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          timeout * US_PER_SEC);
    uint32_t set_time = ztimer_now(ZTIMER_USEC);
    ztimer_set(ZTIMER_EPOCH, &record.timer, timeout);
    DEBUG("time %" PRIu32 " ms: time + %" PRId32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          correction * US_PER_SEC);
    uint32_t now_epoch = ztimer_now(ZTIMER_EPOCH);
    ztimer_adjust_time(ZTIMER_EPOCH, correction);
    TEST_ASSERT(ztimer_now(ZTIMER_EPOCH) == now_epoch + correction);
    TEST_ASSERT_EQUAL_INT(0,
                          ztimer_mutex_lock_timeout(
                              ZTIMER_USEC, &record.lock,
                              (timeout - correction + 1) * US_PER_SEC));
    DEBUG("time %" PRIu32 " ms: record triggered after %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          atomic_load_u32(&record.timeout) - set_time);
    TEST_ASSERT(is_in_range(atomic_load_u32(&record.timeout) - set_time,
                            (timeout - correction) * US_PER_SEC));
}

static void test_adjust_time_forward(void)
{
    record_t record = { .timer = { .callback = callback, .arg = &record },
                        .lock = MUTEX_INIT_LOCKED,
                        .timeout = UINT32_MAX, };

    const uint8_t timeout = 2;
    const int8_t correction = 1;

    DEBUG("time %" PRIu32 " ms: set record in %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          timeout * US_PER_SEC);
    uint32_t set_time = ztimer_now(ZTIMER_USEC);
    ztimer_set(ZTIMER_EPOCH, &record.timer, timeout);
    DEBUG("time %" PRIu32 " ms: time + %" PRId32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          correction * US_PER_SEC);
    uint32_t now_epoch = ztimer_now(ZTIMER_EPOCH);
    ztimer_adjust_time(ZTIMER_EPOCH, correction);
    TEST_ASSERT(ztimer_now(ZTIMER_EPOCH) == now_epoch + correction);
    TEST_ASSERT_EQUAL_INT(0,
                          ztimer_mutex_lock_timeout(
                              ZTIMER_USEC, &record.lock,
                              (timeout - correction + 1) * US_PER_SEC));
    DEBUG("time %" PRIu32 " ms: record triggered after %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          atomic_load_u32(&record.timeout) - set_time);
    TEST_ASSERT(is_in_range(atomic_load_u32(&record.timeout) - set_time,
                            (timeout - correction) * US_PER_SEC));
}

static void test_adjust_time_forward_late(void)
{
    record_t record = { .timer = { .callback = callback, .arg = &record },
                        .lock = MUTEX_INIT_LOCKED,
                        .timeout = UINT32_MAX, };
    const uint8_t timeout = 2;
    const int8_t correction = 3;

    DEBUG("time %" PRIu32 " ms: set record in %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          timeout * US_PER_SEC);
    uint32_t set_time = ztimer_now(ZTIMER_USEC);
    ztimer_set(ZTIMER_EPOCH, &record.timer, timeout);

    DEBUG("time %" PRIu32 " ms: time + %" PRId32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          correction * US_PER_SEC);
    uint32_t now_epoch = ztimer_now(ZTIMER_EPOCH);
    ztimer_adjust_time(ZTIMER_EPOCH, correction);

    TEST_ASSERT(ztimer_now(ZTIMER_EPOCH) == now_epoch + correction);
    TEST_ASSERT_EQUAL_INT(1, mutex_trylock(&record.lock));
    DEBUG("time %" PRIu32 " ms: record triggered after %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          atomic_load_u32(&record.timeout) - set_time);
    TEST_ASSERT(is_in_range(atomic_load_u32(&record.timeout) - set_time, 0));
}

static void test_adjust_time_forward_late_and_advance(void)
{
    record_t record_1 = { .timer = { .callback = callback, .arg = &record_1 },
                          .lock = MUTEX_INIT_LOCKED,
                          .timeout = UINT32_MAX, };
    record_t record_2 = { .timer = { .callback = callback, .arg = &record_2 },
                          .lock = MUTEX_INIT_LOCKED,
                          .timeout = UINT32_MAX, };

    const uint8_t timeout_1 = 2;
    const uint8_t timeout_2 = 4;
    const int8_t correction = 3;

    DEBUG("time %" PRIu32 " ms: set record 1 in %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          timeout_1 * US_PER_SEC);
    DEBUG("time %" PRIu32 " ms: set record 2 in %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          timeout_2 * US_PER_SEC);
    uint32_t set_time = ztimer_now(ZTIMER_USEC);
    ztimer_set(ZTIMER_EPOCH, &record_1.timer, timeout_1);
    ztimer_set(ZTIMER_EPOCH, &record_2.timer, timeout_2);
    DEBUG("time %" PRIu32 " ms: time + %" PRId32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          correction * US_PER_SEC);
    uint32_t now_epoch = ztimer_now(ZTIMER_EPOCH);
    ztimer_adjust_time(ZTIMER_EPOCH, correction);

    TEST_ASSERT(ztimer_now(ZTIMER_EPOCH) == now_epoch + correction);
    TEST_ASSERT_EQUAL_INT(1, mutex_trylock(&record_1.lock));
    DEBUG("time %" PRIu32 " ms: record 1 triggered after %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          atomic_load_u32(&record_1.timeout) - set_time);
    TEST_ASSERT(is_in_range(atomic_load_u32(&record_1.timeout) - set_time, 0));
    TEST_ASSERT_EQUAL_INT(0,
                          ztimer_mutex_lock_timeout(
                              ZTIMER_USEC, &record_2.lock,
                              (timeout_2 - correction + 1) * US_PER_SEC));
    DEBUG("time %" PRIu32 " ms: record 2 triggered after %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          atomic_load_u32(&record_2.timeout) - set_time);
    TEST_ASSERT(is_in_range(atomic_load_u32(&record_2.timeout) - set_time,
                            (timeout_2 - correction) * US_PER_SEC));
}


static void test_adjust_time_set(void)
{
    record_t record = { .timer = { .callback = callback, .arg = &record },
                        .lock = MUTEX_INIT_LOCKED,
                        .timeout = UINT32_MAX, };

    const uint8_t timeout = 2;
    const int8_t correction = 1;

    DEBUG("time %" PRIu32 " ms: time + %" PRId32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          correction * US_PER_SEC);
    uint32_t now_epoch = ztimer_now(ZTIMER_EPOCH);
    ztimer_adjust_time(ZTIMER_EPOCH, correction);

    DEBUG("time %" PRIu32 " ms: set record in %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          timeout * US_PER_SEC);
    uint32_t set_time = ztimer_now(ZTIMER_USEC);
    ztimer_set(ZTIMER_EPOCH, &record.timer, timeout);

    TEST_ASSERT(ztimer_now(ZTIMER_EPOCH) == now_epoch + correction);
    TEST_ASSERT_EQUAL_INT(0,
                          ztimer_mutex_lock_timeout(
                              ZTIMER_USEC, &record.lock,
                              (timeout + 1) * US_PER_SEC));
    DEBUG("time %" PRIu32 " ms: record triggered after %" PRIu32 " ms\n",
          ztimer_now(ZTIMER_USEC),
          atomic_load_u32(&record.timeout) - set_time);
    TEST_ASSERT(is_in_range(atomic_load_u32(&record.timeout) - set_time,
                            (timeout) * US_PER_SEC));
}

Test *tests_adjust_time(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_adjust_time_back),
        new_TestFixture(test_adjust_time_forward),
        new_TestFixture(test_adjust_time_forward_late),
        new_TestFixture(test_adjust_time_forward_late_and_advance),
        new_TestFixture(test_adjust_time_set),
    };

    EMB_UNIT_TESTCALLER(adjust_time_test, setUp, tearDown, fixtures);

    return (Test *)&adjust_time_test;
}


int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_adjust_time());
    TESTS_END();
    return 0;
}
