/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 * @{
 *
 * @file
 * @brief       Glue code for running uwb-core for RIOT
 * *
 * @}
 */

#include "dpl/dpl.h"
#include "dw1000/dw1000_phy.h"
#include "dw1000/dw1000_hal.h"
#include "uwb_dw1000.h"

#include "uwb/uwb.h"
#include "uwb/uwb_mac.h"

#include "utlist.h"
#include "thread.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

#ifndef DW1000_THREAD_PRIORITY
#define DW1000_THREAD_PRIORITY  MYNEWT_VAL_UWB_DEV_TASK_PRIO
#endif

#ifndef DW1000_ROLE_DEFAULT
#define DW1000_ROLE_DEFAULT     0x0
#endif

#ifndef DW1000_RX_DIAGNOSTIC
#define DW1000_RX_DIAGNOSTIC    0
#endif

/* Link list head */
static struct _dw1000_dev_instance_t * dw1000_instance_head;

/* Default instance configuration */
static const dw1000_dev_instance_t dw1000_instance_config_default = {
    .uwb_dev = {
        .idx = 0,
        .role = DW1000_ROLE_DEFAULT,
        .task_prio = DW1000_THREAD_PRIORITY,
        .status = {0},
        .attrib = {
            .nsfd = 8,             /**< Number of symbols in start of frame delimiter */
            .nsync = 128,          /**< Number of symbols in preamble sequence */
            .nphr = 21             /**< Number of symbols in phy header */
        },
        .config = {
            .channel = 5,                       /**< channel number {1, 2, 3, 4, 5, 7 } */
            .prf = DWT_PRF_64M,                 /**< Pulse Repetition Frequency {DWT_PRF_16M or DWT_PRF_64M} */
            .dataRate = DWT_BR_6M8,             /**< Data Rate {DWT_BR_110K, DWT_BR_850K or DWT_BR_6M8} */
            .rx = {
                .pacLength = DWT_PAC8,          /**< Acquisition Chunk Size DWT_PAC8..DWT_PAC64 (Relates to RX preamble length) */
                .preambleCodeIndex = 9,         /**< RX preamble code */
                .sfdType = 1,                   /**< Boolean should we use non-standard SFD for better performance */
                .phrMode = DWT_PHRMODE_EXT,     /**< PHR mode {0x0 - standard DWT_PHRMODE_STD, 0x3 - extended frames DWT_PHRMODE_EXT} */
                .sfdTimeout = (128 + 1 + 8 - 8),/**< SFD timeout value (in symbols) (preamble length + 1 + SFD length - PAC size). Used in RX only. */
                .timeToRxStable = 6,            /**< Time until the Receiver i stable, (in us) */
                .frameFilter = 0,               /**< No frame filtering by default */
                .xtalTrim = 0x10,               /**< Centre trim value */
            },
            .tx ={
                .preambleCodeIndex = 9,         /**< TX preamble code */
                .preambleLength = DWT_PLEN_128  /**< DWT_PLEN_64..DWT_PLEN_4096 */
            },
            .txrf={
                .PGdly = TC_PGDELAY_CH5,
                .BOOSTNORM = dw1000_power_value(DW1000_txrf_config_9db, 2.5),
                .BOOSTP500 = dw1000_power_value(DW1000_txrf_config_9db, 2.5),
                .BOOSTP250 = dw1000_power_value(DW1000_txrf_config_9db, 2.5),
                .BOOSTP125 = dw1000_power_value(DW1000_txrf_config_9db, 2.5)
            },
            .trxoff_enable = 1,
            .rxdiag_enable = DW1000_RX_DIAGNOSTIC,
            .dblbuffon_enabled = 0,
            .LDE_enable = 1,
            .LDO_enable = 0,
            .sleep_enable = 1,
            .wakeup_rx_enable = 1,       /**< Wakeup to Rx state */
            .rxauto_enable = 1,          /**< On error re-enable */
            .cir_enable = 0,             /**< Default behavior for CIR interface */
            .cir_pdoa_slave = 0,         /**< First instance should not act as pdoa slave */
            .blocking_spi_transfers = 1, /**< Nonblocking spi transfers are not supported */
        },
    }
};

void _uwb_dw1000_set_idx(dw1000_dev_instance_t* dev)
{
    int count = 0;
    dw1000_dev_instance_t *elt = NULL;
    LL_COUNT(dw1000_instance_head, elt, count);
    dev->uwb_dev.idx = count++;
    /* prepend to list */
    LL_PREPEND(dw1000_instance_head, dev);
}

void uwb_dw1000_setup(dw1000_dev_instance_t* dev, dw1000_params_t* params)
{
    /* set semaphore */
    dpl_sem_init(params->spi_sem, 0x1);
    /* set default uwb config */
    memcpy(dev, &dw1000_instance_config_default,
           sizeof(dw1000_dev_instance_t));
    /* set uwb_dev idx */
    _uwb_dw1000_set_idx(dev);
    /* this will set the configuration and init uwb_dev which ATM only
       allocates an RX and TX buffer if none is yet available */
    dw1000_dev_init((struct os_dev *) dev, (void *) params);
}

void uwb_dw1000_config_and_start(dw1000_dev_instance_t* dev)
{
    dw1000_dev_config(dev);
}

void uwb_dw1000_set_buffs(dw1000_dev_instance_t* dev, uint8_t* tx_buf,
                          uint8_t* rx_buf)
{
    dev->uwb_dev.rxbuf = rx_buf;
    dev->uwb_dev.txbuf = tx_buf;
}

void uwb_dw1000_init(void)
{
    dw1000_instance_head = NULL;
}

struct uwb_dev* uwb_dev_idx_lookup(int idx)
{
    dw1000_dev_instance_t *current = dw1000_instance_head;

    while(current != NULL)
    {
        if(current->uwb_dev.idx == idx) {
            LOG_DEBUG("uwb_dev: found dev of idx %d\n", idx);
            break;
        }
        current = current->next;
    }

    return (struct uwb_dev*) &current->uwb_dev;
}

/**
 * API to choose DW1000 instances based on parameters.
 *
 * @param idx  Indicates number of instances for the chosen bsp.
 * @return dw1000_dev_instance_t
 */

struct _dw1000_dev_instance_t * hal_dw1000_inst(uint8_t idx)
{
    dw1000_dev_instance_t *current = dw1000_instance_head;

    for (uint8_t i = 0; i < idx; i++) {
        current = current->next;
    }

    return current;
}
