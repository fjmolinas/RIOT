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
#include "rtc_utils.h"
#endif

#include "controller.h"

#ifndef LVGL_STACKSIZE
#define LVGL_STACKSIZE      THREAD_STACKSIZE_LARGE
#endif

#include "slotted_adv.h"
#include "rtc_utils.h"
#include "timex.h"

/* Advertising Event Thread spec */
#ifndef DEFAULT_ADV_ITVL_MS
#define DEFAULT_ADV_ITVL_MS     (1 * MS_PER_SEC)
#endif

#define CURRENT_TIME_SERVICE_UUID16         0x3333

#ifndef CONFIG_TIME_SERVER_ADV_INST
#define CONFIG_TIME_SERVER_ADV_INST         0
#endif
#define CONFIG_BLE_ADV_TX_POWER         127

typedef union __attribute__((packed)) current_time {
    uint32_t epoch;
    uint8_t bytes[4];
} current_time_t;

/* buffer for ad */
static uint8_t buf[BLE_HS_ADV_MAX_SZ];
/* advertising data struct */
static bluetil_ad_t ad;
/* the advertisement event */
static adv_event_t adv_event;
/* the extended adv parameters */
static struct ble_gap_ext_adv_params params;

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

uint32_t get_epoch(void)
{
#if IS_USED(MODULE_DS3231)
    struct tm time;
    ds3231_get_time(&_dev, &time);
    return rtc_mktime(&time);
#else
    return ztimer_now(ZTIMER_SEC);
#endif
}

void set_epoch_adv_data(bluetil_ad_t *ad, void *arg)
{
    (void)arg;

    /* reset buffer */
    memset(ad->buf, 0, ad->size);
    ad->pos = 0;
    /* Tx power field added by the driver */
    extern int ble_phy_txpwr_get(void);
    int8_t phy_txpwr_dbm = ble_phy_txpwr_get();
    int rc = bluetil_ad_add(ad, BLE_GAP_AD_TX_POWER_LEVEL, &phy_txpwr_dbm,
                            sizeof(phy_txpwr_dbm));

    assert(rc == BLUETIL_AD_OK);
    /* Add service data uuid */
    uint16_t svc_uid = CURRENT_TIME_SERVICE_UUID16;

    rc = bluetil_ad_add(ad, BLE_GAP_AD_UUID16_COMP, &svc_uid, sizeof(svc_uid));
    assert(rc == BLUETIL_AD_OK);
    /* Add service data field */
    current_time_t current_time = { .epoch = get_epoch()};
    rc = bluetil_ad_add(ad, BLE_GAP_AD_SERVICE_DATA, &current_time.bytes, sizeof(current_time));
    assert(rc == BLUETIL_AD_OK);
    (void)rc;
    struct tm time;
    rtc_localtime(get_epoch(), &time);
    printf("Current time:\n");
    printf("\tDate: %04d-%02d-%02d %02d:%02d:%02d,\n",
            time.tm_year + 1900,
            time.tm_mon + 1,
            time.tm_mday,
            time.tm_hour,
            time.tm_min,
            time.tm_sec);
    printf("\tEpoch: %"PRIu32"\n", get_epoch());
}

int time_adv_start(void)
{
    slotted_adv_start(&adv_event, DEFAULT_ADV_ITVL_MS, UINT32_MAX, &ad, set_epoch_adv_data, NULL);
    return 0;
}

int time_adv_stop(void)
{
    slotted_adv_stop(&adv_event);
    return 0;
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
    ds3231_params_t params_ds3231 = ds3231_params[0];
    params_ds3231.opt = DS3231_OPT_BAT_ENABLE;
    params_ds3231.opt |= DS3231_OPT_INTER_ENABLE;

    int res = ds3231_init(&_dev, &params_ds3231);
    if (res != 0) {
        puts("error: unable to initialize DS3231 [I2C initialization error]");
        return 1;
    }
#endif

    memset(&params, 0, sizeof(params));
    bluetil_ad_init(&ad, buf, 0, sizeof(buf));
    set_epoch_adv_data(&ad, NULL);
    slotted_adv_init(&adv_event, CONFIG_TIME_SERVER_ADV_INST, EVENT_PRIO_HIGHEST,
                     &params);

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
