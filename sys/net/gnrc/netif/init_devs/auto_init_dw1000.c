/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for the dw1000 network interface
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc.h"

#include "dw1000.h"
#include "dw1000_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define DW1000_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef DW1000_MAC_PRIO
#define DW1000_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define DW1000_NUM ARRAY_SIZE(dw1000_params)

static dw1000_t dw1000_devs[DW1000_NUM];
static char _dw1000_stacks[DW1000_NUM][DW1000_MAC_STACKSIZE];

static gnrc_netif_t _netif[DW1000_NUM];

void auto_init_dw1000(void)
{
    for (unsigned i = 0; i < DW1000_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing dw1000 #%u\n", i);

        dw1000_setup(&dw1000_devs[i], &dw1000_params[i]);
        gnrc_netif_ieee802154_create(&_netif[i], _dw1000_stacks[i],
                                     DW1000_MAC_STACKSIZE, DW1000_MAC_PRIO,
                                     "dw1000",
                                     (netdev_t *)&dw1000_devs[i]);
    }
}
/** @} */
