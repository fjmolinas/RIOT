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
#include "periph/gpio.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


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
        case 0:         /* Stand by mode */

            /* Clear Wakeup flag */    
            PWR->CR |= PWR_CR_CWUF;
        
            /* Select STANDBY mode */
            PWR->CR |= PWR_CR_PDDS;

            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP;

            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;

            /* This option is used to ensure that store operations are completed */
            #if defined (__CC_ARM)
            __force_stores();
            #endif
            
            irq_disable();
            
            /* Request Wait For Interrupt */
            __DSB();
            __WFI();


        case 1:           /* Stop mode */

            /* Disable ADC */
            ADC1->CR2 &= ~ADC_CR2_ADON;
            ADC1->CR1 |= ADC_CR1_PDI;
            ADC1->CR1 |= ADC_CR1_PDD;
            ADC1->CR2 |= ADC_CR2_ADON;
            /* Disable DAC */
            DAC->CR &= ~DAC_CR_EN1;
            DAC->CR &= ~DAC_CR_EN2;
            /* Clear Wakeup flag */    
            PWR->CR |= PWR_CR_CWUF;
            /* Regulator in LP mode */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;
            /* Enable stop mode */
            PWR->CR &= ~PWR_CR_PDDS;
            /* Set SLEEPDEEP bit of Cortex System Control Register */
            SCB->SCR |= (uint32_t)SCB_SCR_SLEEPDEEP;

            irq_disable();

             /* Request Wait For Interrupt */
            __DSB();
            __WFI();

            /* Clear SLEEPDEEP bit */
            SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);

            irq_enable();
            break;

        case 2:              /* Low power sleep mode TODO*/  

             /* Clear Wakeup flag */    
            PWR->CR |= PWR_CR_CWUF;
            /* Enable low-power mode of the voltage regulator */
            PWR->CR |= PWR_CR_LPSDSR;
            /* Clear SLEEPDEEP bit */
            SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP);

            irq_disable();
            
            //*** lpm_before_i_go_to_sleep();

            /* Switch to 65kHz clock */
            //*** switch_to_msi(RCC_ICSCR_MSIRANGE_0, RCC_CFGR_HPRE_DIV1);

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

#if defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
void pm_off(void)
{
    irq_disable();
   
    pm_set(0); 
    
}
#endif


#if defined(CPU_FAM_STM32L1)
void pm_off(void)
{
    irq_disable();
   
    pm_set(0); 
    
}
#endif