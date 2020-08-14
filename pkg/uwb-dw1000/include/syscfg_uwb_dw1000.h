/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb-core
 * @ingroup     uwb-core
 * @{
 *
 * @file
 * @brief       uwb-dw1000 module configurations
 *              taken from @decawave-uwb-dw1000/hw/drivers/uwb/uwb_dw1000 
 *
 */

#ifndef SYSCFG_UWB_DW1000_H
#define SYSCFG_UWB_DW1000_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Used to check that the UWB_DEV has been setup correctly
 *        at compile time.
 * @note  (UWB_DEV_RXDIAG_MAXLEN > 19)
 */
#ifndef MYNEWT_VAL_UWB_DW1000_API_CHECKS
#define MYNEWT_VAL_UWB_DW1000_API_CHECKS (1)
#endif

/**
 * @brief Size of spi read/write buffer, sets the
 *        maximum allowed nonblocking read operation
 */
#ifndef MYNEWT_VAL_DW1000_HAL_SPI_BUFFER_SIZE
#define MYNEWT_VAL_DW1000_HAL_SPI_BUFFER_SIZE (256)
#endif

/**
 * @brief The maximum number of bytes in a single transfer that the
 *        SPI hardware supports. 255 is safe for nrf52.
 */
#ifndef MYNEWT_VAL_DW1000_HAL_SPI_MAX_CNT
#define MYNEWT_VAL_DW1000_HAL_SPI_MAX_CNT (255)
#endif

/**
 * @brief Max size spi read in bytes that is always done with blocking io.
 *        Reads longer than this value will be done with non-blocking io.
 * @note  Ignore in RIOT since non-blocking SPI is not yet enabled
 */
#ifndef MYNEWT_VAL_DW1000_DEVICE_SPI_RD_MAX_NOBLOCK
#define MYNEWT_VAL_DW1000_DEVICE_SPI_RD_MAX_NOBLOCK (9)
#endif

/**
 * @brief Enable range bias correction polynomial
 */
#ifndef MYNEWT_VAL_DW1000_BIAS_CORRECTION_ENABLED
#define MYNEWT_VAL_DW1000_BIAS_CORRECTION_ENABLED (0)
#endif

/**
 * @brief Tx Power dBm
 */
#ifndef MYNEWT_VAL_DW1000_DEVICE_TX_PWR
#define MYNEWT_VAL_DW1000_DEVICE_TX_PWR (((float)-14.3f))
#endif

/**
 * @brief Antenna Gain dB
 */
#ifndef MYNEWT_VAL_DW1000_DEVICE_ANT_GAIN
#define MYNEWT_VAL_DW1000_DEVICE_ANT_GAIN (((float)1.0f))
#endif

/**
 * @brief Enable showing rx and tx activity on leds
 */
#ifndef MYNEWT_VAL_DW1000_RXTX_LEDS
#define MYNEWT_VAL_DW1000_RXTX_LEDS (1)
#endif

/**
 * @brief Enable showing rx and tx activity on gpios
 */
#ifndef MYNEWT_VAL_DW1000_RXTX_GPIO
#define MYNEWT_VAL_DW1000_RXTX_GPIO (0)
#endif

/**
 * @brief Toggle LED_1 for every range packet received
 */
#ifndef MYNEWT_VAL_DW1000_RNG_INDICATE_LED
#define MYNEWT_VAL_DW1000_RNG_INDICATE_LED (0)
#endif

/**
 * @brief Expose event counter cli api
 */
#ifndef MYNEWT_VAL_DW1000_CLI_EVENT_COUNTERS
#define MYNEWT_VAL_DW1000_CLI_EVENT_COUNTERS (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSCFG_UWB_DW1000_H */