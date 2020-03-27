/*
 * Copyright (C) 2019 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, April 2019
 *
 * @}
 */

#ifndef __RADIO_OW_H
#define __RADIO_OW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netdev.h"
#include "radio.h"

/**
 * @brief   Initialize OpenWSN radio
 *
 * @param[in]   dev     pointer to a netdev interface
 *
 * @return  PID of OpenWSN thread
 * @return  -1 on initialization error
 */
int openwsn_radio_init(netdev_t *netdev);

/**
 * @brief openwsn radio variables structure
 */
typedef struct {
    radio_capture_cbt startFrame_cb;  /**< start of frame capture callback */
    radio_capture_cbt endFrame_cb;    /**< end of frame capture callback */
    netdev_t *dev;                    /**< netdev device */
} openwsn_radio_t;

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_OW_H */
/** @} */