/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pepper
 * @defgroup    pkg_uwb_core_twr_gatt   TWR Helper GATT services
 * @brief       Simple GATT service for the TWR helper module
 * @{
 *
 * @file
 *
 * @author      Roudy DAGHER <roudy.dagher@inria.fr>
 *
 * @}
 */

#include <stddef.h>
#include "nimble_autoadv.h"

#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "host/util/util.h"
#include "net/bluetil/ad.h"

#include "net/ieee802154.h"
#include "net/l2util.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "uwb/uwb.h"
#include "uwb_core/twr.h"

#define IEEE802154_SHORT_ADDRESS_LEN_STR_MAX \
    (sizeof("00:00"))

#include "timex.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_INFO
#endif
#include "log.h"

/**
 * @brief   TWR event buffer memory manager
 */
typedef struct uwb_core_twr_data_mem {
    uint8_t buf[CONFIG_UWB_CORE_TWR_DATA_BUF_SIZE * sizeof(uint16_t)];      /**< event buffer */
    memarray_t mem;                                                         /**< Memarray management */
} uwb_core_twr_data_mem_t;
uwb_core_twr_data_mem_t _twr_data_mem;

/** Internal BLE layer **/
/* UUID = 3878e9c0-0000-1000-8000-00805f9b34fb, twr ranging service */
static const ble_uuid128_t gatt_svr_svc_twr_uuid
    = BLE_UUID128_INIT(0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
                       0x00, 0x10, 0x00, 0x00, 0xc0, 0xe9, 0x78, 0x38);

/* UUID = 4dba712c-2f2f-11eb-adc1-0242ac120003 */
static const ble_uuid128_t gatt_svr_chr_twr_request_uuid
    = BLE_UUID128_INIT(0x01, 0x00, 0x12, 0xac, 0x42, 0x02, 0xc1, 0xad,
                       0xeb, 0x11, 0x2f, 0x2f, 0x2c, 0x71, 0xba, 0x4d);

/* UUID = 4dba712c-2f2f-11eb-adc1-0242ac120003 */
static const ble_uuid128_t gatt_svr_chr_twr_listen_uuid
    = BLE_UUID128_INIT(0x02, 0x00, 0x12, 0xac, 0x42, 0x02, 0xc1, 0xad,
                       0xeb, 0x11, 0x2f, 0x2f, 0x2c, 0x71, 0xba, 0x4d);

/* UUID = 4dba712c-2f2f-11eb-adc1-0242ac120004 */
static const ble_uuid128_t gatt_svr_chr_twr_hwaddr_uuid
    = BLE_UUID128_INIT(0x03, 0x00, 0x12, 0xac, 0x42, 0x02, 0xc1, 0xad,
                       0xeb, 0x11, 0x2f, 0x2f, 0x2c, 0x71, 0xba, 0x4d);

/* nimble related structs */
static struct ble_gap_event_listener _gap_event_listener;

/* information about bluetooth connection */
static uint16_t _conn_handle;
static uint16_t _val_handle_stdout;

static int _twr_hwaddr_handler(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _twr_listen_handler(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _twr_request_handler(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

/* define the bluetooth services for our device */
/* GATT service definitions */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: UWB Ranging Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t *)&gatt_svr_svc_twr_uuid,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                /* Characteristic: Read short address*/
                .uuid = (ble_uuid_t *)&gatt_svr_chr_twr_hwaddr_uuid.u,
                .access_cb = _twr_hwaddr_handler,
                .flags =  BLE_GATT_CHR_F_READ,
            },
            {
                /* Characteristic: Execute TWR */
                .uuid = (ble_uuid_t *)&gatt_svr_chr_twr_request_uuid.u,
                .access_cb = _twr_request_handler,
                .flags = BLE_GATT_CHR_F_WRITE,
            },
            {
                /* Characteristic: Read/Write listen state */
                .uuid = (ble_uuid_t *)&gatt_svr_chr_twr_listen_uuid.u,
                .access_cb = &_twr_listen_handler,
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ,
            },
            {
                0, /* No more characteristics in this service */
            },
        }
    },
    {
        0, /* No more services */
    },
};


static int _twr_hwaddr_handler(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    int rc = 0;

    struct uwb_dev *udev = uwb_dev_idx_lookup(0);

    switch (ctxt->op) {

    case BLE_GATT_ACCESS_OP_READ_CHR:
    {
        network_uint16_t hwaddr = byteorder_htons(udev->uid);
        LOG_INFO("[twr] gatt: hwaddr=%" PRIx16 "\n", hwaddr.u16);
        /* send given data to the client */
        rc = os_mbuf_append(ctxt->om, &hwaddr.u8, sizeof(hwaddr));
        break;
    }
    default:
        LOG_WARNING("[twr] gatt: unhandled operation!\n");
        rc = 1;
        break;
    }
    return rc;
}

static int _twr_listen_handler(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    int rc = 0;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        if (uwb_core_twr_listen_is_enabled()) {
            LOG_INFO("[twr] gatt: listen is enabled\n");
            rc = os_mbuf_append(ctxt->om, (uint8_t *)"on", strlen("on"));
        }
        else {
            LOG_INFO("[twr] gatt: listen is disabled\n");
            rc = os_mbuf_append(ctxt->om, (uint8_t *)"off", strlen("off"));
        }
        break;
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        LOG_INFO("[pepper] gatt: set listen ");
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);
        if (om_len != 1) {
            LOG_INFO("error, exceeded buffer size\n");
            rc = 1;
        }
        else {
            uint8_t on;
            rc = ble_hs_mbuf_to_flat(ctxt->om, &on, 1, &om_len);
            if (on) {
                LOG_INFO("on\n");
                uwb_core_twr_listen_enable();
            }
            else {
                LOG_INFO("off\n");
                uwb_core_twr_listen_disable();
            }
        }
        break;
    default:
        LOG_WARNING("[twr] gatt: unhandled operation!\n");
        rc = 1;
        break;
    }
    return rc;
}

static int _twr_request_handler(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    int rc = 0;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        LOG_INFO("[pepper] gatt: request\n");
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);
        if (om_len > sizeof(uint16_t)) {
            LOG_INFO("error, exceeded buffer size\n");
            rc = 1;
        }
        else {
            network_uint16_t hwaddr;
            rc = ble_hs_mbuf_to_flat(ctxt->om, &hwaddr.u8, sizeof(network_uint16_t), &om_len);
            LOG_INFO("[twr] gatt: request to hwaddr=0x%04" PRIx16 "\n", byteorder_ntohs(hwaddr));
            uwb_core_twr_start(byteorder_ntohs(hwaddr), TWR_PROTOCOL_SS, MS_PER_SEC, 1);
        }
        break;
    default:
        LOG_WARNING("[twr] gatt: unhandled operation!\n");
        rc = 1;
        break;
    }
    return rc;
}

// static int _gap_event_cb(struct ble_gap_event *event, void *arg)
// {
//     (void)arg;

//     switch (event->type) {
//     case BLE_GAP_EVENT_CONNECT:
//         _conn_handle = (uint16_t)event_data;
//         LOG_INFO("\tBLE_GAP_CONNECTED: _conn_handle = %d\n", _conn_handle);
//         break;

//     case BLE_GAP_EVENT_DISCONNECT:
//         conn_handle = (uint16_t)event_data;
//         LOG_INFO("\nBLE_GAP_DISCONNECTED: stopping notif\n");
//         if (conn_handle == _conn_handle) {
//             LOG_INFO("\t>>!! Unregistering UWB callback \n");
//             twr_register_rng_cb(NULL);
//         }
//         break;

//     case BLE_GAP_EVENT_SUBSCRIBE:
//         attr_handle = (uint16_t)event_data;
//         LOG_INFO("\nBLE_GAP_SUBSCRIBE: subscribe on handle %d, our handle %d\n", attr_handle,
//                  _rng_val_handle);
//         if (attr_handle == _rng_val_handle) {
//             LOG_INFO("\t>>Registering UWB callback \n");
//             twr_register_rng_cb(&twr_rng_event_cb);
//         }
//         break;
//     default:
//         LOG_ERROR("\nInternal error: unhandled event %d\n", event);
//         break;
//     }
// }

static void _rng_event_cb(uwb_core_twr_data_t *data)
{
    uint16_t* d_cm = (uint16_t*) memarray_calloc(&_twr_data_mem);

    if (notif_rng_data != NULL) {
        memcpy(notif_rng_data, rng_data, sizeof(twr_range_data_t));
        msg.sender_pid = thread_getpid();
        msg.content.ptr = notif_rng_data;
        msg.type = 0xCAFE;
        int rc = msg_send(&msg, listener_pid);
        if (rc != 1) {
            LOG_ERROR("[ERROR] Notif event notification failed: rc=%d\n!", rc);
        }
    }
    else {
        LOG_ERROR("[ERROR] Notif event queue is full !");
    }
}

void uwb_core_twr_gatt_init(void)
{
    int rc = 0;

    /* fix compilation error when using DEVELHELP=0 */
    (void)rc;

    /* verify and add our custom services */
    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    assert(rc == 0);
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    assert(rc == 0);

    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* convert hwaddr to string */
    char addr_str[IEEE802154_SHORT_ADDRESS_LEN_STR_MAX];
    uint8_t buffer[IEEE802154_SHORT_ADDRESS_LEN];
    struct uwb_dev *udev = uwb_dev_idx_lookup(0);

    byteorder_htobebufs(buffer, udev->uid);
    l2util_addr_to_str(buffer, IEEE802154_SHORT_ADDRESS_LEN, addr_str);

    /* start to advertise this node with its hwaddr */
    nimble_autoadv_add_field(BLE_GAP_AD_NAME, addr_str, strlen(addr_str));
    nimble_autoadv_start(NULL);

    /* register gap event listener */
    rc = ble_gap_event_listener_register(&_gap_event_listener, _gap_event_cb, NULL);
    assert(rc == 0);
}
