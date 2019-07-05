/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit SUIT secure firmware updates
 * @ingroup     sys
 * @brief       SUIT secure firmware updates
 *
 * @{
 *
 * @brief       SUIT CoAP helper API
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef SUIT_COAP_H
#define SUIT_COAP_H

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Start SUIT CoAP thread
 */
void suit_coap_run(void);

/**
 * @brief SUIT CoAP endpoint entry.
 *
 * In order to use, include this header, then add SUIT_COAP_SUBTREE to the nanocoap endpoint array.
 * Mind the alphanumerical sorting!
 *
 * See examples/suit_update for an example.
 */
#define SUIT_COAP_SUBTREE \
    { \
        .path="/suit/", \
        .methods=COAP_MATCH_SUBTREE | COAP_METHOD_GET | COAP_METHOD_POST | COAP_METHOD_PUT, \
        .handler=coap_subtree_handler, \
        .context=(void*)&coap_resource_subtree_suit \
    }


/*
 * Dear Reviewer,
 *
 * At the time of PR'ing this code, there was a pile of CoAP PR's waiting for
 * reviews.  Some of that functionality is needed in one way or another for
 * SUIT. In order to not block software updates with CoAP refactoring, some of
 * the work-in-progress code has been copied here.  We expect this to be
 * removed as soon as CoAP in master provides similar functionality.
 *
 * As this is internal code that will go soon, I exclude this from Doxygen.
 *
 * Kaspar (July 2019)
 */
#ifndef DOXYGEN

/* subtree support */
ssize_t coap_subtree_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
                             void *context);

/**
 * @brief   Type for CoAP resource subtrees
 */
typedef const struct {
    const coap_resource_t *resources;   /**< ptr to resource array  */
    const size_t resources_numof;       /**< nr of entries in array */
} coap_resource_subtree_t;

typedef int (*coap_blockwise_cb_t)(void *arg, size_t offset, uint8_t *buf, size_t len, int more);

extern const coap_resource_subtree_t coap_resource_subtree_suit;

/* nanocoap blockwise client */
typedef enum {
    COAP_BLOCKSIZE_32 = 1,
    COAP_BLOCKSIZE_64,
    COAP_BLOCKSIZE_128,
    COAP_BLOCKSIZE_256,
    COAP_BLOCKSIZE_512,
    COAP_BLOCKSIZE_1024,
} coap_blksize_t;

size_t coap_put_option_block(uint8_t *buf, uint16_t lastonum, unsigned blknum,
                             unsigned szx, int more, uint16_t option);

int suit_coap_get_blockwise_url(const char *url,
                               coap_blksize_t blksize,
                               coap_blockwise_cb_t callback, void *arg);

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* SUIT_COAP_H */
/** @} */
