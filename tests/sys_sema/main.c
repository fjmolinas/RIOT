/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terUS and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Inverse Semaphore Test Application
 *
 * @author  Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include "sema.h"
#include "ztimer.h"
#include "timex.h"

#define THREAD_1_POST_DELAY_US          (10 * MS_PER_SEC)
#define THREAD_2_WAIT_TIME_US           (2 * MS_PER_SEC)
#define THREAD_3_WAIT_TIME_US           (4 * MS_PER_SEC)
#define MAIN_THREAD_WAIT_TIME_US        (100 * MS_PER_SEC)

char t1_stack[THREAD_STACKSIZE_SMALL];
char t2_stack[THREAD_STACKSIZE_SMALL];
char t3_stack[THREAD_STACKSIZE_SMALL];
char t4_stack[THREAD_STACKSIZE_SMALL];

struct thread_ctx {
    sema_t *sema;
    unsigned id;
};

static void *thread_1(void *arg)
{
    struct thread_ctx *ctx = arg;

    printf("THREAD %u start\n", ctx->id);

    if (sema_wait(ctx->sema) == 0) {
        printf("THREAD %u sema is posted\n", ctx->id);
    }

    if (sema_try_wait(ctx->sema) == 0) {
        sema_post(ctx->sema);
    }

    ztimer_sleep(ZTIMER_USEC, THREAD_1_POST_DELAY_US);
    printf("THREAD %u wake up, post sema\n", ctx->id);

    sema_post(ctx->sema);

    return arg;
}

static void *thread_2(void *arg)
{
    struct thread_ctx *ctx = arg;

    printf("THREAD %u start\n", ctx->id);

    if (sema_wait_timed(ctx->sema, THREAD_2_WAIT_TIME_US) == -ETIMEDOUT) {
        printf("THREAD %u sema not posted, TIMEOUT\n", ctx->id);
    }
    else {
        sema_post(ctx->sema);
    }

    return arg;
}

static void *thread_3(void *arg)
{
    struct thread_ctx *ctx = arg;

    printf("THREAD %u start\n", ctx->id);

    if (sema_wait_timed_ztimer(ctx->sema, ZTIMER_USEC, THREAD_3_WAIT_TIME_US) == -ETIMEDOUT) {
        printf("THREAD %u sema not posted, TIMEOUT\n", ctx->id);
    }
    else {
        sema_post(ctx->sema);
    }

    return arg;
}

static void *thread_4(void *arg)
{
    struct thread_ctx *ctx = arg;

    printf("THREAD %u start\n", ctx->id);

    if (sema_wait(ctx->sema) == 0) {
        printf("THREAD %u sema is posted\n", ctx->id);
        sema_destroy(ctx->sema);
    }
    else {
        sema_post(ctx->sema);
    }

    return arg;
}

int main(void)
{
    sema_t sema = SEMA_CREATE(1);

    struct thread_ctx ctx[4] = {
        { .sema = &sema, .id = 1 },
        { .sema = &sema, .id = 2 },
        { .sema = &sema, .id = 3 },
        { .sema = &sema, .id = 4 }
    };

    thread_create(t1_stack, sizeof(t1_stack), THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_STACKTEST, thread_1, &ctx[0], "nr1");
    thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, thread_2, &ctx[1], "nr2");
    thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, thread_3, &ctx[2], "nr3");
    thread_create(t4_stack, sizeof(t4_stack), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, thread_4, &ctx[3], "nr4");

    if (sema_wait(&sema) == -ECANCELED) {
        puts("THREAD 0 sema destroyed");
        puts("SUCCESS");
    }
    else {
        puts("FAIL");
    }

    return 0;
}
