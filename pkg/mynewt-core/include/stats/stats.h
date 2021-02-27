/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       Abstraction layer for RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef STATS_STATS_H
#define STATS_STATS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STATS_SECT_DECL(__name)         struct stats_ ## __name
#define STATS_SECT_END                  };

#define STATS_SECT_START(__name)        STATS_SECT_DECL(__name) {
#define STATS_SECT_VAR(__var)

#define STATS_HDR(__sectname)           NULL

#define STATS_SECT_ENTRY(__var)
#define STATS_SECT_ENTRY16(__var)
#define STATS_SECT_ENTRY32(__var)
#define STATS_SECT_ENTRY64(__var)
#define STATS_RESET(__var)

#define STATS_SIZE_INIT_PARMS(__sectvarname, __size) \
                                        0, 0

#define STATS_INC(__sectvarname, __var)
#define STATS_INCN(__sectvarname, __var, __n)
#define STATS_CLEAR(__sectvarname, __var)

#define STATS_NAME_START(__name)
#define STATS_NAME(__name, __entry)
#define STATS_NAME_END(__name)
#define STATS_NAME_INIT_PARMS(__name)   NULL, 0

static inline int
stats_init(void *a, uint8_t b, uint8_t c, void *d, uint8_t e)
{
    /* dummy */
    (void) a;
    (void) b;
    (void) c;
    (void) d;
    (void) e;
    return 0;
}

static inline int
stats_register(void *a, void *b)
{
    /* dummy */
    (void) a;
    (void) b;
    return 0;
}

static inline int
stats_init_and_reg(void *a, uint8_t b, uint8_t c, void *d, uint8_t e, void *f)
{
    /* dummy */
    (void) a;
    (void) b;
    (void) c;
    (void) d;
    (void) e;
    (void) f;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* STATS_STATS_H */
