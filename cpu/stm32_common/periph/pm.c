/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"

#include "cpu.h" // For numbers of ports
#define ENABLE_DEBUG (0)
#include "debug.h"


/* Variables definitions to LPM */
static uint32_t ahb_gpio_clocks;
static uint32_t tmpreg;
static uint16_t lpm_portmask_system[CPU_NUMBER_OF_PORTS] = { 0 };
static uint16_t lpm_portmask_user[CPU_NUMBER_OF_PORTS] = { 0 };

void lpm_before_i_go_to_sleep(void){
    uint8_t i;
    uint8_t p;
    uint32_t mask;
    GPIO_TypeDef *port;

    /* save GPIO clock configuration */
    ahb_gpio_clocks = RCC->AHBENR & 0xFF;
    /* enable all GPIO clocks */
    periph_clk_en(AHB, 0xFF);
    
    for (i = 0; i < CPU_NUMBER_OF_PORTS; i++) {
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));
        mask = 0xFFFFFFFF;
        
        for (p = 0; p < 16; p ++) {
            /* exclude GPIOs registered for external interrupts */
            /* they may be used as wakeup sources */
            if (EXTI->IMR & (1 << p)) {
                if (((SYSCFG->EXTICR[p >> 2]) >> ((p & 0x03) * 4)) == i) {
                    mask &= ~((uint32_t)0x03 << (p*2));
                }
            }
            
            /* exclude GPIOs we previously set with pin_set */
            if ((lpm_portmask_system[i] | lpm_portmask_user[i]) & (1 << p)) {
                mask &= ~((uint32_t)0x03 << (p*2));
            }
        }

        /* disable pull-ups on GPIOs */
        tmpreg = port->PUPDR;
        tmpreg &= ~mask;
        port->PUPDR = tmpreg;
        
        /* set GPIOs to AIN mode */
        tmpreg = port->MODER;
        tmpreg |= mask;
        port->MODER = tmpreg;
        
        /* set lowest speed */
        port->OSPEEDR = 0;
    }
}


void pm_set(unsigned mode)
{
    int deep = 0;

/* I just copied it from stm32f1/2/4, but I suppose it would work for the
 * others... /KS */
#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
    switch (mode) {
        case 0:
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF);
            /* Enable WKUP pin to use for wakeup from standby mode */
            //PWR->CSR |= PWR_CSR_EWUP;
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
        case 1:                 /* STM Stop mode */
            /* Clear PDDS and LPDS bits to enter stop mode on */
            /* deepsleep with voltage regulator on */
            PWR->CR &= ~(PWR_CR_PDDS | PWR_CR_LPDS);
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
    }
#endif

#if defined(CPU_FAM_STM32L1)
    switch (mode) {
        case 0:             /* Stand by mode */

            /* Select STANDBY mode */
            PWR->CR |= PWR_CR_PDDS;
            /* Clear Wakeup flag */    
            PWR->CR |= PWR_CR_CWUF;         
            /* Enable Ultra Low Power mode Ver */
            PWR->CR |= PWR_CR_ULP;
            /* TODO:Clear RTC flag */
            //lpm_before_i_go_to_sleep();
            /* This option is used to ensure that store operations are completed */
            #if defined (__CC_ARM)
            __force_stores();
            #endif
            deep = 1;

        case 1:             /* Stop mode */
            /* Clear PDDS bit */
            PWR->CR &= ~PWR_CR_PDDS;
            /* Clear Wakeup flag */    
            PWR->CR |= PWR_CR_CWUF;
            /* Regulator in LP mode */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;
            //irq_disable();
            lpm_before_i_go_to_sleep();
            deep = 1;
            break;

        case 2:              /* Low power sleep mode TODO*/  

             /* Clear Wakeup flag */    
            PWR->CR |= PWR_CR_CWUF;
            /* Enable low-power mode of the voltage regulator */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Clear SLEEPDEEP bit */
            SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP);

            irq_disable();
            
            /* Request Wait For Interrupt */
            __DSB();
            __WFI();
            
            /* Switch back to default speed */
            //***lpm_select_run_mode(lpm_run_mode);

            //***lpm_when_i_wake_up();
          
            irq_enable();
            deep = 1;
            break;
    }
#endif

    cortexm_sleep(deep);
}

#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32L1)
void pm_off(void)
{
    irq_disable();
   
    pm_set(0); 
    
}
#endif
