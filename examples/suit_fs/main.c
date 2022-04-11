/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       File system usage example application
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "shell.h"
#include "board.h"

#include "xfa.h"
#include "vfs_default.h"

#include "suit/transport/coap.h"
#include "suit/storage.h"
#include "suit/storage/ram.h"
#include "suit/storage/vfs.h"

#include "net/gcoap.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    SUIT_COAP_SUBTREE,
};

static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    GCOAP_SOCKET_TYPE_UNDEF,
    NULL,
    NULL,
    NULL
};

/* add handled storages */
XFA_USE(char*, suit_storage_files_reg);
XFA(suit_storage_files_reg, 0) char* _manifest = VFS_DEFAULT_DATA "/SLOT0.TXT";
XFA(suit_storage_files_reg, 0) char* _firmware = VFS_DEFAULT_DATA "/SLOT1.TXT";

int main(void)
{
    suit_storage_init_all();

#if IS_USED(MODULE_GNRC)
    ipv6_addr_t addr;
    const char addr_str[] = "2001:db8::3";
    ipv6_addr_from_str(&addr, addr_str);
    gnrc_netif_ipv6_addr_add(gnrc_netif_iter(NULL), &addr, 64, 0);
#endif
#if IS_USED(MODULE_LIBCSP)
    csp_iface_t *iface = csp_iflist_get_by_name("can0");
    iface->addr = 0x0022;
#endif

    /* register suit listnet */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_register_listener(&_listener);

    /* start suit coap updater thread */
    suit_coap_run();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
