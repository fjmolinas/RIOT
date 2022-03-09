/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_slotted_adv
 * @{
 *
 * @file
 * @brief       Slotted Advertiser implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "nimble_riot.h"
#include "host/ble_gap.h"

#include "net/bluetil/ad.h"

#include "event.h"
#include "event/callback.h"
#include "event/timeout.h"
#include "event/thread.h"
#include "random.h"
#include "ztimer.h"

#include "slotted_adv.h"

static void _configure_ext_adv(uint8_t instance, struct ble_gap_ext_adv_params *params)
{
    int rc;

    (void)rc;
    /* legacy PDUs do not support higher data rate */
    params->primary_phy = BLE_HCI_LE_PHY_1M;
    params->secondary_phy = BLE_HCI_LE_PHY_1M;
    /* TODO: check if this is the actual preffered tx power, not sure how
       it was handled before */
    params->tx_power = CONFIG_BLE_ADV_TX_POWER;
    /* sid is different from slice id using in desire_ble_pkt */
    params->sid = instance;
    /* use legacy PDUs */
    params->legacy_pdu = 1;
    /* TODO this section could be customizable */
    /* advertise using own */
    params->own_addr_type = nimble_riot_own_addr_type;

    rc = ble_gap_ext_adv_configure(instance, params, NULL, NULL, NULL);
    assert(rc == 0);
}

static void _advertise_once(uint8_t instance, uint8_t *bytes, size_t len)
{
    int rc;

    (void)rc;
    if (ble_gap_ext_adv_active(instance)) {
        rc = ble_gap_ext_adv_stop(instance);
        assert(rc == BLE_HS_EALREADY || rc == 0);
    }

    /* get mbuf for adv data, this is freed from the `ble_gap_ext_adv_set_data` */
    struct os_mbuf *data;

    data = os_msys_get_pkthdr(BLE_HS_ADV_MAX_SZ, 0);
    assert(data);

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, bytes, len);
    assert(rc == 0);
    /* set adv data */
    rc = ble_gap_ext_adv_set_data(instance, data);
    assert(rc == 0);

    /* set a single advertisement event */
    rc = ble_gap_ext_adv_start(instance, ADV_DURATION_MS / 10, 1);
    assert(rc == 0);
}

static void _slotted_adv_handler(void *arg)
{
    adv_event_t *adv_event = (adv_event_t *)arg;

    adv_event->config.advs++;

    /* re-arm timeout if needed, advs remaining or advs forever */
    if (adv_event->config.advs != adv_event->config.advs_max ||
        adv_event->config.advs_max == UINT32_MAX) {
        event_timeout_set(&adv_event->timeout, adv_event->config.itvl_ms);
    }

    /* callback to update advertising payload */
    if (adv_event->config.cb) {
        adv_event->config.cb(adv_event->config.ad, adv_event->config.arg);
    }

    /* advertise once */
    _advertise_once(adv_event->config.instance, adv_event->config.ad->buf,
                    adv_event->config.ad->pos);
}

void slotted_adv_init(adv_event_t *event, uint8_t instance, event_queue_t *queue,
                      struct ble_gap_ext_adv_params *params)
{
    event->queue = queue;
    event->config.instance = instance;

    /* init adv event */
    event_timeout_ztimer_init(&event->timeout, ZTIMER_MSEC, event->queue,
                              &event->event.super);
    event_callback_init(&event->event, _slotted_adv_handler, event);

    /* configure advertisement base parameters */
    _configure_ext_adv(instance, params);
}

void slotted_adv_stop(adv_event_t *event)
{
    if (ble_gap_ext_adv_active(event->config.instance)) {
        int rc = ble_gap_ext_adv_stop(event->config.instance);
        (void)rc;
        assert(rc == BLE_HS_EALREADY || rc == 0);
    }
    event_timeout_clear(&event->timeout);
}

void slotted_adv_start(adv_event_t *event, uint32_t itvl_ms, uint32_t advs_max,
                       bluetil_ad_t *ad, slotted_adv_cb_t cb, void *arg)
{
    /* stop ongoing advertisements if any */
    slotted_adv_stop(event);
    /* set advertisement parameters */
    event->config.advs = 0;
    event->config.advs_max = advs_max;
    event->config.itvl_ms = itvl_ms;
    event->config.cb = cb;
    event->config.arg = arg;
    event->config.ad = ad;
    /* either the callback to set the payload must not be NULL or a payload
       must be already set */
    assert(event->config.cb || event->config.ad->buf);
    /* setup first advertisement event */
    event_timeout_set(&event->timeout, event->config.itvl_ms);
}
