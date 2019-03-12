/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Cortex-M specific configuration and initialization options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"

/**
 * Interrupt vector base address, defined by the linker
 */
extern const void *_isr_vectors;

#if defined(CPU_CORTEXM_INIT_SUBFUNCTIONS)
#define CORTEXM_STATIC_INLINE /*empty*/
#else
#define CORTEXM_STATIC_INLINE static inline
#endif

CORTEXM_STATIC_INLINE void cortexm_init_isr_priorities(void)
{
    /* initialize the interrupt priorities */
    /* set pendSV interrupt to same priority as the rest */
    NVIC_SetPriority(PendSV_IRQn, CPU_DEFAULT_IRQ_PRIO);
    /* set SVC interrupt to same priority as the rest */
    NVIC_SetPriority(SVCall_IRQn, CPU_DEFAULT_IRQ_PRIO);
    /* initialize all vendor specific interrupts with the same value */
    for (unsigned i = 0; i < CPU_IRQ_NUMOF; i++) {
        NVIC_SetPriority((IRQn_Type) i, CPU_DEFAULT_IRQ_PRIO);
    }
}

CORTEXM_STATIC_INLINE void cortexm_init_misc(void)
{
    /* enable wake up on events for __WFE CPU sleep */
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

    /* for Cortex-M3 r1p0 and up the STKALIGN option was added, but not automatically
     * enabled until revision r2p0. For 64bit function arguments to work properly this
     * needs to be enabled.
     */
#ifdef SCB_CCR_STKALIGN_Msk
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
#endif
}

void cortexm_init(void)
{
    cortexm_init_fpu();

    /* configure the vector table location to internal flash */
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F) || defined(CPU_ARCH_CORTEX_M7) || \
    (defined(CPU_ARCH_CORTEX_M0PLUS) || defined(CPU_ARCH_CORTEX_M23) \
    && (__VTOR_PRESENT == 1))
    SCB->VTOR = (uint32_t)&_isr_vectors;
#endif

    cortexm_init_isr_priorities();
    cortexm_init_misc();
}

uint8_t cpu_check_address(volatile const char *address)
{
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || defined(CPU_ARCH_CORTEX_M4F)
    uint8_t is_valid = 1;

    /* Clear BFAR ADDRESS VALID flag */
    SCB->CFSR |= SCB_CFSR_BFARVALID;

    /*
     * We want to turn off the hard fault interrupt because if we try to acces a invalid address
     * we will get a Bus Fault withouth triggering the interrupt, which we can then use to know
     * if the address checkes is valid
     */
    /* Ignore data BusFaults caused by load and store instructions */
    SCB->CCR |= SCB_CCR_BFHFNMIGN;
    /* Turn off hard fault interrupt */
    __asm volatile ("cpsid f;");

    /* Read address and check BFAR ADDRESS VALID flag*/
    *address;
    if ((SCB->CFSR & SCB_CFSR_BFARVALID) != 0)
    {
        /* Bus Fault occured reading the address */
        is_valid = 0;
    }

    /* Turn on hard fault interrupt */
    __asm volatile ("cpsie f;");
    SCB->CCR &= ~SCB_CCR_BFHFNMIGN;

    return is_valid;
#else
    /* Cortex-M0 doesn't have BusFault */
    (void) address;
    printf("[warning] %s: Cortex-M0 doesn't have BusFault\n", __func__);
    return 1;
#endif
}
