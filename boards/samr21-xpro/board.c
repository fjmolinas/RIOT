/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr21-xpro
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM R21 Xplained
 *              Pro board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

#if defined(MODULE_MTD_SDCARD) || defined(DOXYGEN)
#include "mtd_sdcard.h"
#include "sdcard_spi.h"
#include "sdcard_spi_params.h"
 /* this is provided by the sdcard_spi driver
  * see drivers/sdcard_spi/sdcard_spi.c */
extern sdcard_spi_t sdcard_spi_devs[ARRAY_SIZE(sdcard_spi_params)];
mtd_sdcard_t samr21_xpro_dev = {
    .base = {
        .driver = &mtd_sdcard_driver,
    },
    .sd_card = &sdcard_spi_devs[0],
    .params = &sdcard_spi_params[0]
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&samr21_xpro_dev;

#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
VFS_AUTO_MOUNT(fatfs, VFS_MTD(samr21_xpro_dev), VFS_DEFAULT_NVM(0), 0);
#endif
#endif /* MODULE_MTD_SDCARD || DOXYGEN */


void board_antenna_config(uint8_t antenna)
{
    if (antenna == RFCTL_ANTENNA_EXT){
        gpio_set(RFCTL1_PIN);
        gpio_clear(RFCTL2_PIN);
    }
    else if (antenna == RFCTL_ANTENNA_BOARD){
        gpio_clear(RFCTL1_PIN);
        gpio_set(RFCTL2_PIN);
    }
}

void board_init(void)
{

    /* initialize the on-board antenna switch */
    if (IS_USED(MODULE_AT86RF233)) {
        gpio_init(RFCTL1_PIN, GPIO_OUT);
        gpio_init(RFCTL2_PIN, GPIO_OUT);

        /* set default antenna switch configuration */
        board_antenna_config(RFCTL_ANTENNA_DEFAULT);
    }
}
