/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "assert.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#include "periph/flashpage.h"

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
/* Program memory unlock keys */
#define FLASH_PRGKEY1          ((uint32_t)0x8C9DAEBF)
#define FLASH_PRGKEY2          ((uint32_t)0x13141516)
#define CNTRL_REG              (FLASH->PECR)
#define CNTRL_REG_LOCK         (FLASH_PECR_PELOCK)
#define FLASH_CR_PER           (FLASH_PECR_ERASE | FLASH_PECR_PROG)
#define FLASHWRITE_DIV         (4U) /* write 4 bytes in one go */
#else
#if defined(CPU_FAM_STM32L4)
#define FLASHWRITE_DIV         (8U)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7)
#define FLASHSECTORS_BANK      (12)
#define FLASHWRITE_DIV         (4U)
#else
#define FLASHWRITE_DIV         (2U)
#endif
#define CNTRL_REG              (FLASH->CR)
#define CNTRL_REG_LOCK         (FLASH_CR_LOCK)
#endif

extern void _lock(void);
extern void _unlock(void);
extern void _wait_for_pending_operations(void);

static void _unlock_flash(void)
{
    _unlock();

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    DEBUG("[flashpage] unlocking the flash program memory\n");
    if (!(CNTRL_REG & CNTRL_REG_LOCK)) {
        if (CNTRL_REG & FLASH_PECR_PRGLOCK) {
            DEBUG("[flashpage] setting the program memory unlock keys\n");
            FLASH->PRGKEYR = FLASH_PRGKEY1;
            FLASH->PRGKEYR = FLASH_PRGKEY2;
        }
    }
#endif
}

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
static inline int _flashbank_sector(void *addr)
{
    /* When flash is in single bank there is a max of 12 sectors. The first 4
       sectors are equally sized, the 5th amount to the sum of the first 4
       sectors and the 6th to 12th amount to the sum of the first 5 sectos.
       e.g.: if FLASHSECTOR_SIZE_MIN is 16k there are 4 sectors of 16kB,
       1 of 64kB and 7 of 128kB. */
    /* We use this pattern to find in what sector and address falls */
    uint8_t sn =
        (uint8_t)(((uint32_t)addr - CPU_FLASH_BASE) / FLASHSECTOR_SIZE_MIN);

    if (sn > 3 && sn < 8) {
        sn = 4;
    }
    else if (sn >= 8) {
        sn = (sn / 8) + 4;
    }
    return sn;
}

uint32_t flashsector_size(uint8_t sn) {
#if (FLASH_DUAL_BANK == 1)
    if (sn == FLASHSECTOR_NUMOF) {
        return FLASHSECTOR_SIZE_MIN;
    }
    else {
        return (((uint32_t) flashsector_addr(sn + 1)) - ((uint32_t) flashsector_addr(sn)));
    }
#else
    return (((uint32_t) flashsector_addr(sn + 1)) - ((uint32_t) flashsector_addr(sn)));
#endif
}

void* flashsector_addr(uint8_t sn)
{
/* Assert on Invalid Sector */
#if (FLASH_DUAL_BANK == 1)
    assert((sn <= (int) FLASHSECTOR_NUMOF) || \
           ((sn >= (int) FLASHSECTORS_BANK)) && \
            (sn <= (int) (FLASHSECTOR_NUMOF + FLASHSECTORS_BANK)));
#else
    assert(sn <= (int) FLASHSECTOR_NUMOF);
#endif

    uint32_t addr = CPU_FLASH_BASE;
#if (FLASH_DUAL_BANK == 1)
    if (sn >= FLASHSECTORS_BANK) {
       sn %= FLASHSECTORS_BANK;
       addr += (STM32_FLASHSIZE / 2);
    }
#endif
    if (sn <= 4) {
        addr += (FLASHSECTOR_SIZE_MIN * sn);
    }
    else {
        addr += (8*FLASHSECTOR_SIZE_MIN * (sn - 4));
    }
    return (void *) addr;
}

int flashsector_sector(void *addr)
{
    /* When in dual bank there can be up to 24 sectors, where sectors 12-23
       follow the same layout as sectors 0-11 */
#if (FLASH_DUAL_BANK == 1)
    if ((uint32_t)addr >= (STM32_FLASHSIZE / 2) + CPU_FLASH_BASE) {
        DEBUG("[flashsector]: dual bank sector\n");
        addr = (void *)((uint32_t)addr - (STM32_FLASHSIZE / 2));
        return FLASHSECTORS_BANK + _flashbank_sector(addr);
    }
    else {
        DEBUG("[flashsector]: single bank sector\n");
        return _flashbank_sector(addr);
    }
#else
    return _flashbank_sector(addr);
#endif
}

void flashpage_erase_raw(void *page_addr)
{
    /* Erases must be sector aligned */
    assert((unsigned) page_addr == flashsector_addr(sn));
    /* Make addres is contained in flash */
    assert((unsigned) page_addr < (CPU_FLASH_BASE + STM32_FLASHSIZE);

    uint8_t sn = flashsector_sector(page_addr);

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    /* unlock the flash module */
    _unlock_flash();

    /* set parallelism to 32bits */
    CNTRL_REG &= FLASH_CR_PSIZE_Msk;
    CNTRL_REG |= (0x2 << FLASH_CR_PSIZE_Pos);

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    DEBUG("[flashsector] erase: setting the sector erase code\n");
    CNTRL_REG |= ((sn % FLASHSECTORS_BANK) << FLASH_CR_SNB_Pos);
#if (FLASH_DUAL_BANK == 1)
    CNTRL_REG |= (sn / FLASHSECTORS_BANK) * FLASH_CR_SNB_4;
#endif
    DEBUG("[flashsector] erase: setting the erase bit\n");
    CNTRL_REG |= FLASH_CR_SER;

    DEBUG("[flashsector] erase: trigger the page erase\n");
    CNTRL_REG |= FLASH_CR_STRT;

    /* wait as long as device is busy */
    _wait_for_pending_operations();

    /* reset PER bit */
    DEBUG("[flashsector] erase: resetting the sector erase bit\n");
    CNTRL_REG &= ~FLASH_CR_SER;

    /* lock the flash module again */
    _lock();
}
#endif

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32L4)
void flashpage_erase_raw(void *page_addr)
{
    /* Erases must be flash page aligned */
    assert((unsigned) page_addr % FLASHPAGE_SIZE);

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || \
    defined(CPU_FAM_STM32L4)
    uint32_t *dst = page_addr;
#else
    uint16_t *dst = page_addr;
#endif
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);
    /* the internal RC oscillator (HSI) must be enabled */
    stmclk_enable_hsi();
#endif

    /* unlock the flash module */
    _unlock_flash();

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

    /* set page erase bit and program page address */
    DEBUG("[flashpage] erase: setting the erase bit\n");
    CNTRL_REG |= FLASH_CR_PER;
    DEBUG("address to erase: %p\n", page_addr);
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    DEBUG("[flashpage] erase: trigger the page erase\n");
    *dst = (uint32_t)0;
#elif defined(CPU_FAM_STM32L4)
    DEBUG("[flashpage] erase: setting the page address\n");
    uint8_t pn;
#if FLASHPAGE_NUMOF <= 256
    pn = (uint8_t)flashpage_page(dst);
#else
    uint16_t page = flashpage_page(dst);
    if (page > 255) {
        CNTRL_REG |= FLASH_CR_BKER;
    }
    else {
        CNTRL_REG &= ~FLASH_CR_BKER;
    }
    pn = (uint8_t)page;
#endif
    CNTRL_REG &= ~FLASH_CR_PNB;
    CNTRL_REG |= (uint32_t)(pn << FLASH_CR_PNB_Pos);
    CNTRL_REG |= FLASH_CR_STRT;
#else /* CPU_FAM_STM32F0 || CPU_FAM_STM32F1 || CPU_FAM_STM32F3 */
    DEBUG("[flashpage] erase: setting the page address\n");
    FLASH->AR = (uint32_t)dst;
    /* trigger the page erase and wait for it to be finished */
    DEBUG("[flashpage] erase: trigger the page erase\n");
    CNTRL_REG |= FLASH_CR_STRT;
#endif
    /* wait as long as device is busy */
    _wait_for_pending_operations();

    /* reset PER bit */
    DEBUG("[flashpage] erase: resetting the page erase bit\n");
    CNTRL_REG &= ~(FLASH_CR_PER);

    /* lock the flash module again */
    _lock();

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
    /* restore the HSI state */
    if (!hsi_state) {
        stmclk_disable_hsi();
    }
#endif
}
#endif

void flashpage_write_raw(void *target_addr, const void *data, size_t len)
{
    /* assert multiples of FLASHPAGE_RAW_BLOCKSIZE are written and no less of
       that length. */
    assert(!(len % FLASHPAGE_RAW_BLOCKSIZE));

    /* ensure writes are aligned */
    assert(!(((unsigned)target_addr % FLASHPAGE_RAW_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_RAW_ALIGNMENT)));

    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)) + 1);

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1)
    uint32_t *dst = target_addr;
    const uint32_t *data_addr = data;
#elif defined(CPU_FAM_STM32L4)
    uint64_t *dst = target_addr;
    const uint64_t *data_addr = data;
#else
    uint16_t *dst = (uint16_t *)target_addr;
    const uint16_t *data_addr = data;
#endif

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
    uint32_t hsi_state = (RCC->CR & RCC_CR_HSION);
    /* the internal RC oscillator (HSI) must be enabled */
    stmclk_enable_hsi();
#endif

    /* unlock the flash module */
    _unlock_flash();

    /* make sure no flash operation is ongoing */
    _wait_for_pending_operations();

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7)
    /* set parallelism to 32bits */
    CNTRL_REG &= FLASH_CR_PSIZE_Msk;
    CNTRL_REG |= (0x2 << FLASH_CR_PSIZE_Pos);
#endif

    DEBUG("[flashpage_raw] write: now writing the data\n");
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32L4)
    /* set PG bit and program page to flash */
    CNTRL_REG |= FLASH_CR_PG;
#endif
    for (size_t i = 0; i < (len / FLASHWRITE_DIV); i++) {
        DEBUG("[flashpage_raw] writing %c to %p\n", (char)data_addr[i], dst);
        *dst++ = data_addr[i];
#if defined(CPU_FAM_STM32F7)
        __DSB();
#endif
        /* wait as long as device is busy */
        _wait_for_pending_operations();
    }

    /* clear program bit again */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32L4)
    CNTRL_REG &= ~(FLASH_CR_PG);
#endif
    DEBUG("[flashpage_raw] write: done writing data\n");

    /* lock the flash module again */
    _lock();

#if !(defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || \
      defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F2) || \
      defined(CPU_FAM_STM32F4))
    /* restore the HSI state */
    if (!hsi_state) {
        stmclk_disable_hsi();
    }
#endif
}

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1)
void flashpage_write(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
    /* STM32L0/L1 only supports word sizes */
    uint32_t *page_addr = flashpage_addr(page);
#elif defined(CPU_FAM_STM32L4)
    uint64_t *page_addr = flashpage_addr(page);
#else
    /* Default is to support half-word sizes */
    uint16_t *page_addr = flashpage_addr(page);
#endif

    /* ERASE sequence */
    flashpage_erase_raw(page_addr);

    /* WRITE sequence */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }
}
#endif
