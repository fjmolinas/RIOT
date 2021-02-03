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
#include "dpl_syscfg/syscfg_uwb.h"

/*** @decawave-uwb-core/lib/twr_ds */
#include "dpl_syscfg/syscfg_twr_ds.h"

/*** @decawave-uwb-core/lib/twr_ds_ext */
#include "dpl_syscfg/syscfg_twr_ds_ext.h"

/*** @decawave-uwb-core/lib/twr_ss */
#include "dpl_syscfg/syscfg_twr_ss.h"

/*** @decawave-uwb-core/lib/twr_ss_ack */
#include "dpl_syscfg/syscfg_twr_ss_ack.h"

/*** @decawave-uwb-core/lib/twr_ss_ext */
#include "dpl_syscfg/syscfg_twr_ss_ext.h"

/*** @decawave-uwb-core/lib/uwb_rng */
#include "dpl_syscfg/syscfg_uwb_rng.h"

/*** @decawave-uwb-core/sys/uwbcfg */
#include "dpl_syscfg/syscfg_uwbcfg.h"
#endif

#if IS_USED(MODULE_UWB_DW1000)
/*** @decawave-uwb-dw1000/hw/drivers/uwb/uwb_dw1000 */
#include "syscfg_uwb_dw1000.h"
#endif

#if IS_USED(MODULE_NIMBLE)
/*** @mynewt-nimble */
#include "syscfg_nimble.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSCFG_SYSCFG_H */
