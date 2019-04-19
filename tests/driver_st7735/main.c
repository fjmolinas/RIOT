/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
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
 * @brief       Test application for the ili9431 tft display
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include "byteorder.h"
#include "xtimer.h"
#include "st7735.h"
#include "st7735_params.h"

#include "riot_logo.h"

int main(void)
{
    st7735_t dev;
    
    puts("st7735 TFT display test application");

    /* initialize the sensor */
    printf("Initializing display...");

    if (st7735_init(&dev, &st7735_params[0]) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    puts("st7735 TFT display filling map");
    st7735_fill(&dev, 0, 127, 0, 127, 0x0000);
    puts("st7735 TFT display map filled");

    puts("Drawing blue rectangle");
    st7735_fill(&dev, 0, 63, 0, 63, 0x001F);
    xtimer_sleep(1);

    puts("Drawing green rectangle");
    st7735_fill(&dev, 64, 127, 0, 63, 0x07E0);
    xtimer_sleep(1);
    
    puts("Drawing red rectangle");
    st7735_fill(&dev, 0, 63, 64, 127, 0xF000);
    xtimer_sleep(1);
    
    puts("Drawing yellow rectangle");
    st7735_fill(&dev, 64, 127, 64, 127, 0xFE80);
    xtimer_sleep(1);

    st7735_map(&dev, 0, 127, 0, 68, (const uint16_t *)picture);

    while (1) {
    }

    return 0;
}