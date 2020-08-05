/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "net/gcoap.h"


#include "openwsn.h"
#include "fmt.h"
#include "opendefs.h"
#include "02a-MAClow/IEEE802154E.h"
#include "02b-MAChigh/neighbors.h"
#include "03b-IPv6/icmpv6rpl.h"
#ifdef MODULE_OPENWSN_CJOIN
#include "cjoin.h"
#endif
#include "cross-layers/idmanager.h"

#define MAIN_QUEUE_SIZE     (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern idmanager_vars_t idmanager_vars;
extern icmpv6rpl_vars_t icmpv6rpl_vars;
extern neighbors_vars_t neighbors_vars;

extern int gcoap_cli_cmd(int argc, char **argv);
extern void gcoap_cli_init(void);
static int _ifconfig_cmd(int argc, char **argv);

char addr_str[IPV6_ADDR_MAX_STR_LEN];

static const shell_command_t shell_commands[] = {
    { "coap", "CoAP example", gcoap_cli_cmd },
    { "ifconfig", "Shows assigned IPv6 addresses", _ifconfig_cmd },
    { NULL, NULL, NULL }
};

char *_array_2_string(const uint8_t *addr, size_t addr_len, char *out)
{
    char *res = out;

    assert((out != NULL) && ((addr != NULL) || (addr_len == 0U)));
    out[0] = '\0';
    for (size_t i = 0; i < addr_len; i++) {
        out += fmt_byte_hex((out), *(addr++));
        *(out++) = (i == (addr_len - 1)) ? '\0' : ':';
    }
    return res;
}

static int _ifconfig_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    open_addr_t *hwaddr;
    open_addr_t temp_my128bID;

    memcpy(&temp_my128bID.addr_128b[0], &idmanager_vars.myPrefix.prefix, 8);
    memcpy(&temp_my128bID.addr_128b[8], &idmanager_vars.my64bID.addr_64b, 8);

    printf("Iface  %d  ", openwsn_get_pid());
    hwaddr = idmanager_getMyID(ADDR_16B);
    printf("\tHWaddr: %s  ", _array_2_string(hwaddr->addr_16b, 2, addr_str));
    hwaddr = idmanager_getMyID(ADDR_PANID);
    printf("NID: %s\n", _array_2_string(hwaddr->panid, 2, addr_str));
    printf("\n");

    hwaddr = idmanager_getMyID(ADDR_64B);
    printf("\t\tLong HWaddr: %s\n", _array_2_string(hwaddr->addr_64b, 8, addr_str));

    ipv6_addr_to_str(addr_str, (ipv6_addr_t *)temp_my128bID.addr_128b,
                     sizeof(addr_str));
    printf("\t\tinet6 addr: %s\n", addr_str);
    printf("\n");


    printf("\t\tIEEE802154E sync: %i\n", ieee154e_isSynch());
#ifdef MODULE_OPENWSN_CJOIN
    printf("\t\t6TiSCH joined: %i\n", cjoin_getIsJoined());
#endif
    printf("\n");

    if (idmanager_vars.isDAGroot) {
        puts("\t\tNode is DAG root");
    }
    else {
        if (icmpv6rpl_vars.haveParent) {
            printf("\t\tRPL rank: %i\n", icmpv6rpl_vars.myDAGrank);
            printf("\t\tRPL parent: %s\n", \
                   _array_2_string(neighbors_vars.neighbors[icmpv6rpl_vars.
                                                            ParentIndex].
                                   addr_64b.addr_64b, 8, addr_str));
            printf("\t\tRPL children:\n");
            for (uint8_t i = 0; i < MAXNUMNEIGHBORS; i++) {
                if ((neighbors_isNeighborWithHigherDAGrank(i)) == true) {
                    printf("\t\t\t%s\n", \
                        _array_2_string(neighbors_vars.neighbors[i].
                        addr_64b.addr_64b, 8, addr_str));
                }
            }
            ipv6_addr_to_str(addr_str,
                             (ipv6_addr_t *)icmpv6rpl_vars.dao.DODAGID,
                             sizeof(addr_str));
            printf("\t\tRPL DODAG ID: %16s\n", addr_str);
        }
        else {
            puts("\t\tNO RPL parent");
        }
    }
    return 0;
}

int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_cli_init();
    puts("gcoap example app");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}