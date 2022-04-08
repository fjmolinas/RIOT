/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_libcsp
 * @{
 *
 * @file
 * @brief       LibCSP RIOT types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef CSP_AUTOCONFIG_H
#define CSP_AUTOCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define CSP_HAVE_STDIO    1

#define CSP_QFIFO_LEN 8
#define CSP_PORT_MAX_BIND 7
#define CSP_CONN_RXQUEUE_LEN 8
#define CSP_CONN_MAX 4
#define CSP_BUFFER_SIZE 255
#define CSP_BUFFER_COUNT 8
#define CSP_RDP_MAX_WINDOW 5
#define CSP_RTABLE_SIZE 5

#ifdef __cplusplus
}
#endif

#endif /* CSP_AUTOCONFIG_H */
