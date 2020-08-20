/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
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
 * @brief       OpenWSN test application
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "od.h"
#include "fmt.h"
#include "shell.h"
#include "net/ipv6/addr.h"
#include "errno.h"

#include "openwsn.h"
#include "openwsn_sap.h"

#include "opendefs.h"
#include "02a-MAClow/IEEE802154E.h"
#include "02b-MAChigh/sixtop.h"
#include "02b-MAChigh/schedule.h"

static int txtsend(int argc, char **argv)
{
    uint8_t addr[8];
    if(argc > 2) {
        fmt_hex_bytes(addr, argv[1]);
        openwsn_mcps_data_request((char*) addr, argv[2], strlen(argv[2]));
    }
    else {
        openwsn_mcps_data_request(NULL, argv[1], strlen(argv[1]));
    }

    return 0;
}

static int slotframe_cmd(int argc, char **argv)
{
    (void) argc;
    openwsn_mlme_set_slotframe_request(atoi(argv[1]));
    return 0;
}

int role_cmd(int argc, char **argv)
{
    (void) argc;

    char *role = argv[1];
    if(strcmp(role, "pancoord") == 0) {
        openwsn_mlme_set_role(ROLE_PAN_COORDINATOR);
    } else if(strcmp(role, "coord") == 0) {
        openwsn_mlme_set_role(ROLE_COORDINATOR);
    }
    else if(strcmp(role, "leaf") == 0){
        openwsn_mlme_set_role(ROLE_LEAF);
    }
    else {
        printf("Usage: %s <pancoord|coord|leaf>\n", argv[0]);
    }
    puts("OK");
    return 0;
}

/* Callback from MAC layer */
void openwsn_mcps_data_confirm(int status)
{
    if(status == 0) {
        puts("Successfully sent data");
    }
    else if (status == -ENOBUFS) {
        puts("Not enough buffer space");
    }
    else {
        puts("Error while sending packet");
    }
}

/* Callback from MAC layer */
void openwsn_mcps_data_indication(char *data, size_t data_len)
{
    printf("Received message: ");
    for(unsigned i=0;i<data_len;i++) {
        printf("%c", data[i]);
    }
    puts("");
}

static const shell_command_t shell_commands[] = {
    { "slotframe", "Set slotframe length", slotframe_cmd },
    { "txtsnd", "Set node as RPL DODAG root node", txtsend },
    { "role", "Set role of node", role_cmd},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("OpenWSN MAC test");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}