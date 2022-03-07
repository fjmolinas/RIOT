/*
 * Copyright (C) 2019 Inria
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
 * @brief       LittlevGL example application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "thread.h"
#include "event/thread.h"

#if IS_USED(MODULE_DS3231)
#include "ds3231.h"
#include "ds3231_params.h"
#else
#include "ztimer.h"
#include "periph/rtc.h"
#endif

#include "controller.h"

#ifndef LVGL_STACKSIZE
#define LVGL_STACKSIZE      THREAD_STACKSIZE_LARGE
#endif

extern void *lvgl_thread(void *arg);
static char _lvgl_stack[LVGL_STACKSIZE];

#if IS_USED(MODULE_DS3231)
static ds3231_t _dev;
#endif

void get_time(struct tm *time)
{
#if IS_USED(MODULE_DS3231)
    ds3231_get_time(&_dev, time);
#else
    rtc_localtime(ztimer_now(ZTIMER_SEC), time);
#endif
}

#if IS_USED(MODULE_DS3231)
int _cmd_time(int argc, char **argv)
{
    if (argc == 1) {
        struct tm time;
        ds3231_get_time(&_dev, &time);
        printf("Current time:\n");
        printf("\tDate: %04d-%02d-%02d %02d:%02d:%02d,\n",
               time.tm_year + 1900,
               time.tm_mon + 1,
               time.tm_mday,
               time.tm_hour,
               time.tm_min,
               time.tm_sec);
    }
    else if (argc == 7) {
        struct tm time;
        time.tm_year = (uint16_t)(atoi(argv[1])) - 1900;
        time.tm_mon = (uint8_t)(atoi(argv[2])) - 1;
        time.tm_mday = (uint8_t)(atoi(argv[3]));
        time.tm_hour = (uint8_t)(atoi(argv[4]));
        time.tm_min = (uint8_t)(atoi(argv[5]));
        time.tm_sec = (uint8_t)(atoi(argv[6]));
        printf("New time:\n");
        printf("\tDate: %04d-%02d-%02d %02d:%02d:%02d,\n",
               time.tm_year + 1900,
               time.tm_mon + 1,
               time.tm_mday,
               time.tm_hour,
               time.tm_min,
               time.tm_sec);
        ds3231_set_time(&_dev, &time);
    }
    else {
        puts("usage: time <year> <month=[1 = January, 12 = December]> <day> <hour> "
             "<min> <sec>");
        return -1;
    }
    return 0;
}
#endif

static const shell_command_t _commands[] = {
#if IS_USED(MODULE_DS3231)
    { "time", "set/get time", _cmd_time },
#endif
    { NULL, NULL, NULL }
};

int main(void)
{
    /* initialize the device */
#if IS_USED(MODULE_DS3231)
    ds3231_params_t params = ds3231_params[0];
    params.opt = DS3231_OPT_BAT_ENABLE;
    params.opt |= DS3231_OPT_INTER_ENABLE;

    int res = ds3231_init(&_dev, &params);
    if (res != 0) {
        puts("error: unable to initialize DS3231 [I2C initialization error]");
        return 1;
    }
#endif

    /* start lvgl server thread */
    thread_create(_lvgl_stack, sizeof(_lvgl_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  lvgl_thread, NULL, "lvgl");

    /* initiate controller */
    controller_init(EVENT_PRIO_HIGHEST);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
