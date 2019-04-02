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
#include "xtimer.h"

int setup_wdg(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <min_time[us]> <max_time[us]>\n", argv[0]);
        return -1;
    }
    
    uint32_t min_time = (uint32_t)atoi(argv[1]);
    uint32_t max_time = (uint32_t)atoi(argv[2]);

    if (min_time > max_time)
    {
        puts("[wdg]: invalid configuration boundaries");
        return -1;
    }

    int result = wdg_setup(min_time, max_time);

    if (!result) {
        puts("[wdg]: - wdg configured -");
    }
    else {
        puts("[wdg]: - invalid configuration time -");
    }
    return result;
}

int start_wdg(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("[wdg]: starting wdg timer");
    wdg_start();
    return 0;
}

int start_loop_wdg(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("json:{\"starttime\":%lu}\n", xtimer_now_usec());
    wdg_start();
    while(1)
    {
        printf("json:{\"time\":%lu}\n", xtimer_now_usec());
    }
    return 0;
}

int stop_wdg(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("[wdg]: stoping wdg timer");
    wdg_stop();
    return 0;
}

int kick_wdg(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("[wdg]: delaying wdg timer");
    wdg_kick();
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "setup", "Setup Wdg Timer", setup_wdg },
    { "kick", "Delay wdg timer", kick_wdg },
    { "start", "Start wdg timer", start_wdg },
    { "startloop", "Start wdg timer", start_loop_wdg },
    { "stop", "Start wdg timer", stop_wdg },
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
