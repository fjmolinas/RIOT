/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      
 *
 * @}
 */


#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#ifdef CPU_FAM_STM32L1

static uint32_t gpio_pm_moder[8];
static uint16_t gpio_pm_otyper[8];
static uint32_t gpio_pm_pupdr[8];
#if defined (STM32L1XX_HD) || defined (STM32L1XX_XL)
    static uint32_t gpio_pm_brr[8];
#endif
void gpio_pm_save(void)
{
    GPIO_TypeDef *port_addr;
	
	for (uint32_t i = 0; i < 8; i++) {
        port_addr = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));

        if (cpu_check_address((char *)port_addr)) {
            /* save GPIO registers values */
           gpio_pm_moder[i] = port_addr->MODER;
           gpio_pm_pupdr[i] = port_addr->PUPDR;
           gpio_pm_otyper[i] = (uint16_t)(port_addr->OTYPER & 0xFFFF);
            #if defined (STM32L1XX_HD) || defined (STM32L1XX_XL)
               gpio_pm_brr[i] = (uint16_t)(port_addr->BRR & 0xFFFF);
            #endif
        } else {
            break;
        }
	}
}

void gpiom_pm_add_exception(gpio_t pin))
{
    // adds an exception to the save mask
}

void gpiom_pm_remove_exception(gpio_t pin))
{
    // adds an exception to the save mask
}

void gpio_pm_restore(void)
{
    GPIO_TypeDef *port_addr;
	
	for (uint32_t i = 0; i < 8; i++) {
        port_addr = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));

        if (cpu_check_address((char *)port_addr)) {
            /* save GPIO registers values */
            port_addr->MODER = gpio_pm_moder[i];
            port_addr->PUPDR = gpio_pm_pupdr[i];
            port_addr->OTYPER = gpio_pm_otyper[i]
            #if defined (STM32L1XX_HD) || defined (STM32L1XX_XL)
                port_addr->BRR = gpio_pm_brr;
            #endif
        } else {
            break;
        }
	}
}

void gpio_pm_init(void)
{
    uint32_t ahb_gpio_clocks;
    uint32_t tmpreg;
    GPIO_TypeDef *port;

    /* enable GPIO clock and save GPIO clock configuration */
    ahb_gpio_clocks = RCC->AHBENR & 0xFF;
    periph_clk_en(AHB, 0xFF);

    /* switch all GPIOs to AIN*/
    for (uint8_t i = 0; i < 8; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));
        if (cpu_check_address((char *)port)) {
            port->MODER = 0xffffffff;
        } else {
            break;
        }
    }

    /* restore GPIO clock */
    tmpreg = RCC->AHBENR;
    tmpreg &= ~((uint32_t)0xFF);
    tmpreg |= ahb_gpio_clocks;
    periph_clk_en(AHB, tmpreg);
}
#endif
