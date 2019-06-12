/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2048 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f4 STM32F4
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F4
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#include "vendor/stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_LINE_STM32F401xE)
#define CPU_IRQ_NUMOF                   (85U)
#elif defined(CPU_LINE_STM32F405xx) || defined(CPU_LINE_STM32F407xx) \
    || defined(CPU_LINE_STM32F415xx)
#define CPU_IRQ_NUMOF                   (82U)
#elif defined(CPU_LINE_STM32F410Rx)
#define CPU_IRQ_NUMOF                   (98U)
#elif defined(CPU_LINE_STM32F411xE)
#define CPU_IRQ_NUMOF                   (86U)
#elif defined(CPU_LINE_STM32F412Zx) || defined(CPU_LINE_STM32F446xx)
#define CPU_IRQ_NUMOF                   (97U)
#elif defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)
#define CPU_IRQ_NUMOF                   (102U)
#elif defined(CPU_LINE_STM32F429xx) || defined(CPU_LINE_STM32F437xx)
#define CPU_IRQ_NUMOF                   (91U)
#endif
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief   Flash sector configuration
 *
 *          NOTE: STM32F4 flash is organized in sectors instead of pages
 *
 * @{
 */
/* Dual bank layout for STM32F4 with 2Mbyte of flash */
#if (defined(CPU_LINE_STM32F429xx) || defined(CPU_LINE_STM32F437xx)) && \
    (STM32_FLASHSIZE == (2048*1024))
#define FLASH_DUAL_BANK           (1)
#else
#define FLASH_DUAL_BANK           (0)
#endif
#define FLASHSECTOR_SIZE_MIN      (16*1024U)
/* Flash sectors are counted per flash bank */
#if FLASH_DUAL_BANK == 0
#define FLASHSECTOR_NUMOF         (4 + (STM32_FLASHSIZE / (8 * FLASHSECTOR_SIZE_MIN)))
#else
#define FLASHSECTOR_NUMOF         (4 + (STM32_FLASHSIZE / (8 * 2 * FLASHSECTOR_SIZE_MIN))
#endif
/* The minimum block size which can be written is 4B. However, the erase
 * depends on the specific sector.
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (4U)
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (4U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
