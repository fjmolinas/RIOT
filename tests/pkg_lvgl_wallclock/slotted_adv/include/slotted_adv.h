/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_slotted_adv Slotted Advertiser
 * @ingroup     sys
 * @brief       An Nimble Auto Advertiser with dynamic payloads
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef SLOTTED_ADV_H
#define SLOTTED_ADV_H

#include "nimble_riot.h"
#include "host/ble_gap.h"

#include "net/bluetil/ad.h"

#include "event.h"
#include "event/callback.h"
#include "event/timeout.h"
#include "event/thread.h"
#include "random.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_TIME_SERVER_ADV_INST
#define CONFIG_TIME_SERVER_ADV_INST         0
#endif
#define CONFIG_BLE_ADV_TX_POWER         127

/**
 * @brief The extended advertisement event duration
 *
 * This value should not matter, it will timeout because of the amount
 * of configured event
 */
#define ADV_DURATION_MS 1000

/* advertising data struct */
typedef void (*slotted_adv_cb_t)(bluetil_ad_t *ad, void *arg);

/**
 * @brief   Advertising managemer struct
 */
typedef struct {
    uint32_t itvl_ms;                           /**< the advertisement interval in ms */
    uint32_t advs;                              /**< current event count */
    uint32_t advs_max;                          /**< the total amount of advertisements */
    uint8_t instance;
    bluetil_ad_t *ad;
    slotted_adv_cb_t cb;
    void *arg;
} adv_config_t;

/**
 * @brief   Advertising event
 */
typedef struct {
    event_queue_t *queue;
    event_timeout_t timeout;    /**< the event timeout */
    event_callback_t event;     /**< the event callback */
    adv_config_t config;        /**< the advertsing manager */
} adv_event_t;

void slotted_adv_init(adv_event_t *event, uint8_t instance, event_queue_t *queue,
                      struct ble_gap_ext_adv_params *params);

void slotted_adv_stop(adv_event_t *event);

void slotted_adv_start(adv_event_t *event, uint32_t itvl_ms, uint32_t advs_max,
                       bluetil_ad_t *ad, slotted_adv_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* SLOTTED_ADV_H */
/** @} */
