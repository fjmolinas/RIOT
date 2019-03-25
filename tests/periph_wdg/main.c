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
#include <string.h>
#include <stdlib.h>

#include "periph/wdg.h"
#include "timex.h"
#include "shell.h"

int setup_wdg(int argc, char **argv){
    if (argc < 2){
        printf("usage: %s <time[us]>\n", argv[0]);
        return 1;
    }

    uint32_t rst_time = wdg_init((uint32_t) atoi(argv[1]));
    printf("[wdg]: configured with reset time %lu [us]\n", rst_time);
    return 0;
}

int start_wdg(int argc, char **argv){
    (void) argc;
    (void) argv;
    printf("[wdg]: started wdg timer\n");
    wdg_enable();

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Setup Wdg Timer", setup_wdg },
    { "start", "Start wdg timer", start_wdg },
    { NULL, NULL, NULL }
};

int main(void)
{
    printf("RIOT wdg test application\n");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
