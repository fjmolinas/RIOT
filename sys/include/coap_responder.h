/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_coap_responder EDHOC Coap Responder
 * @ingroup     sys
 * @brief       An EDHOC Coap Responder
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef COAP_RESPONDER_H
#define COAP_RESPONDER_H

#include "coap.h"
#include "edhoc/edhoc.h"
// #include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TODO:
 * edhoc/genkey: - generate rpk/auth key on compilation
 *               - add to credentials
 *               - store generated rpk with an id
 *               - call it only on deployment build?
 *
 *
 * cred_db:      - credential database module
 *               - read from a global file included though cflags
 */

/**
 *  pseudo generic edhoc_c wrappers
 *
 * Reasonable to expect same configuration for different contexts
 *
 *
 * TODO:
 *     * first step: pre-generate keys/rpk
 *     * check when cidr needs t
 *     * edhoc_conf_setup_ad_callbacks
 *     * edhoc_conf_setup_role
 *     * edhoc_conf_setup_credentials
 *     * edhoc_ctx_setup
 */


typedef void (*edhoc_resp_callback_t)(edhoc_ctx_t *, void *);

typedef struct coap_responder_ctx {
    // clist_node_t node;
    uint8_t token[COAP_TOKEN_LENGTH_MAX];
    size_t token_len;
    edhoc_ctx_t *ctx;
    edhoc_resp_callback_t *cb;
    void* arg;
} coap_responder_ctx_t;

int coap_responder_edhoc_setup(edhoc_ctx_t *ctx, edhoc_conf_t *conf,
                               cose_key_t *auth_key, cred_id_t *cred_id, rpk_t *rpk,
                               void *hash_ctx);

int coap_responder_add_ctx(coap_responder_ctx_t *ctx);

int coap_responder_remove_ctx(coap_responder_ctx_t *ctx);

static inline void coap_responder_ctx_init(coap_responder_ctx_t *ctx,
                                           edhoc_ctx_t *edhoc_ctx,
                                           edhoc_respp_callback_t *cb, void8 arg,
                                           uint8_t *token, size_t token_len)
{
    ctx->cb = cb;
    ctx->ctx = edhoc_ctx;
    memcpy(ctx->token, token, token_len);
    ctx->token_len = token_len;
    ctx->arg = arg;
}

coap_responder_ctx_t *coap_responder_get_ctx(uint8_t *token, size_t token_len);

#ifdef __cplusplus
}
#endif

#endif /* COAP_RESPONDER_H */
/** @} */
