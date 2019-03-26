/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for wdg Drivers
 *
 *              This test initializes the wdg counter to a preset value
 *              and spins until a wdg reset is triggered
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "periph/wdg.h"
#include "shell.h"

int init_wdg(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <time[us]>\n", argv[0]);
        return -1;
    }
    uint32_t rst_time = (uint32_t)atoi(argv[1]);
    int result = wdg_init(rst_time);

    if (!result) {
        puts("[wdg]: wdg configured");
    }
    else {
        puts("[wdg]: invalid configuration time");
    }
    return result;
}

int start_wdg(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("[wdg]: started wdg timer");
    wdg_enable();
    return 0;
}

int range_wdg(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("[wdg] range - { \"max\":\"%lu\", \"min\":\"%lu\"} - [wdg] range\n", \
           wdg_max_timeout(), wdg_min_timeout());
    return 0;
}


static const shell_command_t shell_commands[] = {
    { "range", "Get wdg range for reset time", range_wdg },
    { "init", "Setup Wdg Timer", init_wdg },
    { "start", "Start wdg timer", start_wdg },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT wdg test application");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
