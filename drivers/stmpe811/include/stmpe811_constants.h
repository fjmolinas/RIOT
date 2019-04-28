/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stmpe811
 *
 * @{
 * @file
 * @brief       Constants for STMPE811
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef STMPE811_CONSTANTS_H
#define STMPE811_CONSTANTS_H

#include "board.h"
#include "stmpe811.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STMPE811_CHIP_ID_VALUE          (0x0811) /**< Chip ID */

/* @name    Registers
 * @{
 */
#define STMPE811_CHIP_ID                (0x00) /**< STMPE811 Device identification */
#define STMPE811_ID_VER	                (0x02) /**< STMPE811 Revision number */
#define STMPE811_SYS_CTRL1              (0x03) /**< Reset control */
#define STMPE811_SYS_CTRL2              (0x04) /**< Clock control */
#define STMPE811_SPI_CFG                (0x08) /**< SPI interface configuration */
#define STMPE811_INT_CTRL               (0x09) /**< Interrupt control register */
#define STMPE811_INT_EN	                (0x0A) /**< Interrupt enable register */
#define STMPE811_INT_STA                (0x0B) /**< Interrupt status register */
#define STMPE811_GPIO_EN                (0x0C) /**< GPIO interrupt enable register */
#define STMPE811_GPIO_INT_STA           (0x0D) /**< GPIO interrupt status register */
#define STMPE811_ADC_INT_EN             (0x0E) /**< ADC interrupt enable register */
#define STMPE811_ADC_INT_STA            (0x0F) /**< ADC interface status register */
#define STMPE811_GPIO_SET_PIN           (0x10) /**< GPIO set pin register */
#define STMPE811_GPIO_CLR_PIN           (0x11) /**< GPIO clear pin register */
#define STMPE811_MP_STA	                (0x12) /**< GPIO monitor pin state register */
#define STMPE811_GPIO_DIR               (0x13) /**< GPIO direction register */
#define STMPE811_GPIO_ED                (0x14) /**< GPIO edge detect register */
#define STMPE811_GPIO_RE                (0x15) /**< GPIO rising edge register */
#define STMPE811_GPIO_FE                (0x16) /**< GPIO falling edge register */
#define STMPE811_GPIO_AF                (0x17) /**< Alternate function register */
#define STMPE811_ADC_CTRL1              (0x20) /**< ADC control */
#define STMPE811_ADC_CTRL2              (0x21) /**< ADC control */
#define STMPE811_ADC_CAPT               (0x22) /**< To initiate ADC data acquisition */
#define STMPE811_ADC_DATA_CHO           (0x30) /**< ADC channel 0 */
#define STMPE811_ADC_DATA_CH1           (0x32) /**< ADC channel 1 */
#define STMPE811_ADC_DATA_CH2           (0x34) /**< ADC channel 2 */
#define STMPE811_ADC_DATA_CH3           (0x36) /**< ADC channel 3 */
#define STMPE811_ADC_DATA_CH4           (0x38) /**< ADC channel 4 */
#define STMPE811_ADC_DATA_CH5           (0x3A) /**< ADC channel 5 */
#define STMPE811_ADC_DATA_CH6           (0x3C) /**< ADC channel 6 */
#define STMPE811_ADC_DATA_CH7           (0x3E) /**< ADC channel 7 */
#define STMPE811_TSC_CTRL               (0x40) /**< 4-wire tsc setup */
#define STMPE811_TSC_CFG                (0x41) /**< Tsc configuration */
#define STMPE811_WDW_TR_X               (0x42) /**< Window setup for top right X */
#define STMPE811_WDW_TR_Y               (0x44) /**< Window setup for top right Y */
#define STMPE811_WDW_BL_X               (0x46) /**< Window setup for bottom left X */
#define STMPE811_WDW_BL_Y               (0x48) /**< Window setup for bottom left Y */
#define STMPE811_FIFO_TH                (0x4A) /**< FIFO level to generate interrupt */
#define STMPE811_FIFO_STA               (0x4B) /**< Current status of FIFO */
#define STMPE811_FIFO_SIZE              (0x4C) /**< Current filled level of FIFO */
#define STMPE811_TSC_DATA_X             (0x4D) /**< Data port for tsc data access */
#define STMPE811_TSC_DATA_Y             (0x4F) /**< Data port for tsc data access */
#define STMPE811_TSC_DATA_Z             (0x51) /**< Data port for tsc data access */
#define STMPE811_TSC_DATA_XYZ           (0x52) /**< Data port for tsc data access */
#define STMPE811_TSC_FRACTION_Z         (0x56) /**< Touchscreen controller FRACTION_Z */
#define STMPE811_TSC_DATA               (0x57) /**< Data port for tsc data access */
#define STMPE811_TSC_I_DRIVE            (0x58) /**< Touchscreen controller drivel */
#define STMPE811_TSC_SHIELD             (0x59) /**< Touchscreen controller shield */
#define STMPE811_TEMP_CTRL              (0x60) /**< Temperature sensor setup */
#define STMPE811_TEMP_DATA              (0x61) /**< Temperature data access port */
#define STMPE811_TEMP_TH                (0x62) /**< Threshold for temp controlled int */
/** @} */

/* @name    Bitfields
 * @{
 */
#define STMPE811_SYS_CTRL2_ADC_OFF      (1 << 0)
#define STMPE811_SYS_CTRL2_TSC_OFF      (1 << 1)
#define STMPE811_SYS_CTRL2_GPIO_OFF     (1 << 2)
#define STMPE811_SYS_CTRL2_TS_OFF       (1 << 3)

#define STMPE811_TEMP_CTRL_EN           (1 << 0)
#define STMPE811_TEMP_CTRL_ACQ          (1 << 1)
#define STMPE811_TEMP_DATA_MSB_MASK     (0x03)

#define STMPE811_TSC_CTRL_EN            (1 << 0)

#define STMPE811_INT_EN_TOUCH_DET       (1 << 0)
#define STMPE811_INT_EN_FIFO_TH         (1 << 1)
#define STMPE811_INT_EN_FIFO_OFLOW      (1 << 2)
#define STMPE811_INT_EN_FIFO_FULL       (1 << 3)
#define STMPE811_INT_EN_FIFO_EMPTY      (1 << 4)
#define STMPE811_INT_EN_TEMP_SENS       (1 << 5)
#define STMPE811_INT_EN_ADC             (1 << 6)
#define STMPE811_INT_EN_GPIO            (1 << 7)

#define STMPE811_SYS_CTRL1_SOFT_RESET   (1 << 1)

#define STMPE811_FIFO_STA_TOUCH_DET     (1 << 0)

#define STMPE811_INT_CTRL_GLOBAL_INT    (1 << 0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* STMPE811_CONSTANTS_H */
/** @} */
