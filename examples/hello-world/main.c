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
#include "uart_stdio.h"

uint8_t reboot = 0;

uint8_t dummy = 8;

void callback(void)
{
    LED0_TOGGLE;
    reboot = 1;
}

int main(void)
{
    reboot = 0;

    dummy = 5;

    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);
    puts("Hello World!");

    gpio_init_int(GPIO_PIN(PORT_C, 13), GPIO_IN_PU, GPIO_FALLING, (gpio_cb_t)callback, NULL);

    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_ON;

    xtimer_sleep(3);
    puts("Hello World!");

    pm_set(0);

    while(1)
    {
        if(reboot)
        {
            gpio_init(LED0_PIN, GPIO_OUT);
            cpu_init();
            uart_stdio_init();
            xtimer_sleep(3);
            gpio_init(LED0_PIN, GPIO_OUT);
            LED0_OFF;
            printf("Dummy: %d",dummy);
            puts("Pressed BTN \n");
            xtimer_sleep(3);
            reboot = 0;
            pm_set(0);
        }


    }

    return 0;
}
