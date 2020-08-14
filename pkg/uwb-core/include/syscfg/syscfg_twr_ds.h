/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb-core
 * @ingroup     uwb-core
 * @{
 *
 * @file
 * @brief       uwb-dw100 double side two-way ranging module configurations
 *              taken from @decawave-uwb-core/lib/twr_ds/syscfg.yml
 *
 */

#ifndef SYSCFG_TWR_DS_H
#define SYSCFG_TWR_DS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable toplevel ranging services
 */
#ifndef MYNEWT_VAL_TWR_DS_ENABLED
#define MYNEWT_VAL_TWR_DS_ENABLED (IS_ACTIVE(MODULE_UWB_CORE_TWR_DS))
#endif

/**
 * @brief TOA timeout delay for DS TWR (usec)
 */
#ifndef MYNEWT_VAL_TWR_DS_RX_TIMEOUT
#define MYNEWT_VAL_TWR_DS_RX_TIMEOUT (((uint16_t)0x30))
#endif

/**
 * @brief tx holdoff delay for DS TWR (usec)
 */
#ifndef MYNEWT_VAL_TWR_DS_TX_HOLDOFF
#define MYNEWT_VAL_TWR_DS_TX_HOLDOFF (((uint32_t)0x0300))
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSCFG_TWR_DS_H */
