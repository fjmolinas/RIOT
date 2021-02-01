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
 * @brief       uwb-core DPL (Decawave Porting Layer) error types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef MYNEWT_ERROR_H
#define MYNEWT_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DPL error types
 */
enum mynewt_error {
    MYNEWT_OK = 0,
    MYNEWT_ENOMEM = 1,
    MYNEWT_EINVAL = 2,
    MYNEWT_INVALID_PARAM = 3,
    MYNEWT_MEM_NOT_ALIGNED = 4,
    MYNEWT_BAD_MUTEX = 5,
    MYNEWT_TIMEOUT = 6,
    MYNEWT_ERR_IN_ISR = 7,
    MYNEWT_ERR_PRIV = 8,
    MYNEWT_OS_NOT_STARTED = 9,
    MYNEWT_ENOENT = 10,
    MYNEWT_EBUSY = 11,
    MYNEWT_ERROR = 12,
};

/**
 * @brief   dep error type
 */
typedef enum mynewt_error mynewt_error_t;

#ifdef __cplusplus
}
#endif

#endif /* MYNEWT_ERROR_H */
