/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the STMPE811 touchscreen controller
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"

#include "stmpe811.h"
#include "stmpe811_params.h"

int main(void)
{
    stmpe811_t dev;

    puts("STMPE811 test application\n");

    printf("+------------Initializing------------+\n");
    int ret = stmpe811_init(&dev, &stmpe811_params[0]);
    if (ret != STMPE811_OK) {
        puts("[Error] Initialization failed");
        return 1;
    }

    puts("Initialization successful");

    uint8_t current_pressed = stmpe811_read_touch(&dev);
    uint8_t last_pressed = current_pressed;
    int16_t x, y;

    while (1) {
        current_pressed = stmpe811_read_touch(&dev);
        if (current_pressed != last_pressed) {
            if (current_pressed == STMPE811_STATE_PRESSED) {
                stmpe811_read_xy(&dev, &x, &y);
                printf("Pressed! X: %i, Y:%i\n", x, y);
            }
            else {
                puts("Released!");
            }
            last_pressed = current_pressed;
        }

        xtimer_usleep(10 * US_PER_MS);
    }

    return 0;
}
