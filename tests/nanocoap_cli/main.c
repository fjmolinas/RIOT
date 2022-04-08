/*
 * Copyright (c) 2018 Ken Bannister. All rights reserved.
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
 * @brief       nanocoap test app
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"

#include "net/gnrc/netif.h"
#include "net/ipv6/addr.h"
#include "shell.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int nanotest_client_cmd(int argc, char **argv);
extern int nanotest_server_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "client", "CoAP client", nanotest_client_cmd },
    { "server", "CoAP server", nanotest_server_cmd },
    { NULL, NULL, NULL }
};


int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("nanocoap test app");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
