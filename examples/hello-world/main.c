/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "periph/pm.h"
#include "xtimer.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "uart_stdio.h"
#include "shell.h"
#include "msg.h"

#include "auto_init.h"
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern void auto_init_mrf24j40(void);

extern int udp_cmd(int argc, char **argv);

int sleep(int argc, char **argv)
{
    puts("Going sleep");
    gpio_clear(GPIO_PIN(PORT_C, 0));
    xtimer_sleep(0.5);
    pm_set(0);
    return 1;
}

int radio_start(int argc, char **argv)
{
    puts("Radio start");
    auto_init_mrf24j40();
    return 1;
}

static const shell_command_t shell_commands[] = {
    { "radio", "start radio", radio_start},
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { "sleep", "send uC to sleep", sleep},
    { NULL, NULL, NULL }
};

void on_start(void)
{
    cpu_init();
    uart_stdio_init();
    gpio_set(GPIO_PIN(PORT_C, 0));
    // spi_init_cs(SPI_DEV(0),GPIO_PIN(PORT_B, 6));
    // gpio_init(GPIO_PIN(PORT_B, 4), GPIO_OUT);
    // gpio_set(GPIO_PIN(PORT_B, 4));
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
}

void callback(void)
{
    // LED0_TOGGLE;
    on_start();
}

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    gpio_init_int(GPIO_PIN(PORT_C, 13), GPIO_IN_PU, GPIO_FALLING, (gpio_cb_t)callback, NULL);
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
