/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core system configurations
  *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef SYSCFG_SYSCFG_H
#define SYSCFG_SYSCFG_H

#include "kernel_defines.h"

/**
 * @name    MyNewt header inclusion macro definitions
 * @{
 *
 * This macro exists to ensure code includes this header when needed.  If code
 * checks the existence of a setting directly via ifdef without including this
 * header, the setting macro will silently evaluate to 0.  In contrast, an
 * attempt to use these macros without including this header will result in a
 * compiler error.
 */
#define MYNEWT_VAL(_name)                       MYNEWT_VAL_ ## _name
#define MYNEWT_VAL_CHOICE(_name, _val)          MYNEWT_VAL_ ## _name ## __ ## _val
/** @} */

#if IS_USED(MODULE_NIMBLE)
/***  include nimble here later */
#endif

#if IS_USED(MODULE_UWB_CORE)
/*** @decawave-uwb-core/hw/drivers/uwb */
#include "sycfg/syscfg_uwb.h"

/*** @decawave-uwb-core/lib/twr_ds */
#include "sycfg/syscfg_twr_ds.h"

/*** @decawave-uwb-core/lib/twr_ds_ext */
#include "sycfg/syscfg_twr_ds_ext.h"

/*** @decawave-uwb-core/lib/twr_ss */
#include "sycfg/syscfg_twr_ss.h"

/*** @decawave-uwb-core/lib/twr_ss_ack */
#include "sycfg/syscfg_twr_ss_ack.h"

/*** @decawave-uwb-core/lib/twr_ss_ext */
#include "sycfg/syscfg_twr_ss_ext.h"

/*** @decawave-uwb-core/lib/uwb_rng */
#include "sycfg/syscfg_uwb_rng.h"

/*** @decawave-uwb-core/sys/uwbcfg */
#include "sycfg/syscfg_uwbcfg.h"

#if IS_USED(MODULE_UWB_DW1000)
/*** @decawave-uwb-dw1000/hw/drivers/uwb/uwb_dw1000 */
#include "sycfg/syscfg_uwb_dw1000.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSCFG_SYSCFG_H */
