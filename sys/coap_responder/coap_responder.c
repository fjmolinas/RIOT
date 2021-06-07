/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_coap_responder
 * @{
 *
 * @file
 * @brief       EDHOC Coap Responder implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include "net/nanocoap_sock.h"
#include "coap_responder.h"

/* generate this file with make */
#include "edhoc_keys.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* TODO: convert to a linked list instead of single refernce */
static coap_responder_ctx_t *_head;

ssize_t _edhoc_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    (void)context;
    ssize_t msg_len = -1;
    unsigned int error_code;

    DEBUG("[coap_responder]: received an EDHOC message (len %d):\n", pkt->payload_len);
    /* TODO: this should return a new context if ther are available in a statically
             allocated memarray otherwise fail*/
    coap_responder_ctx_t *ctx = coap_responder_get(pkt->token, coap_get_token_len(pkt));

    if (!ctx) {
        DEBUG_PUTS("[coap_responder]: not ctx to allocate");
        /* TODO: should be COAP_CODE_SERVICE_UNAVAILABLE and set retry-after */
        error_code = COAP_CODE_INTERNAL_SERVER_ERROR;
        goto error;
    }

    if (ctx->ctx->state == EDHOC_WAITING) {
        uint8_t msg2[COAP_BUF_SIZE];
        if ((msg_len =
                 edhoc_create_msg2(ctx->ctx, pkt->payload, pkt->payload_len, msg2,
                                   sizeof(msg2))) >= 0) {
            DEBUG("[coap_responder]: sending msg2 (%d bytes):\n", (int)msg_len);
            msg_len = coap_reply_simple(pkt, COAP_CODE_204, buf, len, COAP_FORMAT_OCTET, msg2,
                                        msg_len);
            goto exit;
        }
        else {
            DEBUG_PUTS("[coap_responder]: failed to create msg2");
            error_code = COAP_CODE_BAD_REQUEST;
            goto error;
        }
    }
    else if (ctx.state == EDHOC_SENT_MESSAGE_2) {
        DEBUG_PUTS("[coap_responder]: finalize exchange");
        edhoc_resp_finalize(ctx->ctx, pkt->payload, pkt->payload_len, false, NULL, 0);
        msg_len = coap_reply_simple(pkt, COAP_CODE_204, buf, len, COAP_FORMAT_OCTET, NULL, 0);
        if (ctx->ctx->state == EDHOC_FINALIZED) {
            DEBUG_PUTS("[coap_responder]: handshake completed");
            if (ctx->cb) {
                ctx->cb(ctx->ctx, ctx->arg);
            }
            goto exit;
        }
    }

    if (ctx->ctx->state == EDHOC_FAILED) {
        DEBUG_PUTS("[coap_responder]: protocol error");
        /* TODO: are edhoc error codes implemented ?? */
        error = COAP_CODE_BAD_REQUEST;
        goto error;
    }

error:
    coap_reply_simple(pkt, error_code, buf, len, COAP_FORMAT_TEXT, NULL, 0);
exit:
    if (ctx && (ctx->ctx->state == EDHOC_FAILED ||
                ctx->ctx->state == EDHOC_FINALIZED)) {
        /* TODO: free allocated context */
    }
    return msg_len;
}

coap_responder_ctx_t *coap_responder_get_ctx(uint8_t *token, size_t token_len)
{
    /* TODO: do same operation on whole list */
    if (token_len == _head->token_len) {
        if (memcmp(_head->token, token, token_len) == 0) {
            return _head];
        }
    }
    return NULL;
}

int coap_responder_add_ctx(coap_responder_ctx_t *ctx)
{
    /* TODO: add to a linked list */
    _head = ctx;
    return 0;
}

int coap_responder_rmv_ctx(coap_responder_ctx_t *ctx)
{
    /* TODO: remove from a linked list */
    _head = NULL;
    return 0;
}

int _cred_cb(const uint8_t *k, size_t k_len, const uint8_t **o, size_t *o_len)
{
    for (uint8_t i = 0; i < (uint8_t)CRED_DB_SIZE; i++) {
        if (cred_db[i].id_len == k_len) {
            if (memcmp(cred_db[i].id, k, k_len) == 0) {
                *o = cred_db[i].cred;
                *o_len = cred_db[i].cred_len;
                return 0;
            }
        }
    }
    *o = NULL;
    *o_len = 0;
    return EDHOC_ERR_INVALID_CRED_ID;
}

int coap_responder_edhoc_setup(edhoc_ctx_t *ctx, edhoc_conf_t *conf,
                               cose_key_t *auth_key, cred_id_t *cred_id, rpk_t *rpk,
                               void *hash_ctx)
{
    edhoc_ctx_init(ctx);
    edhoc_conf_init(conf);
    cred_id_init(cred_id);
    cred_rpk_init(rpk);
    cose_key_init(auth_key);
    /* TODO: do not hardcode resp_cbor_rpk, resp_rpk_id, resp_auth_key) */
    DEBUG_PUTS("[coap_responder]: load private authentication key");
    if (cose_key_from_cbor(auth_key, resp_cbor_auth_key, sizeof(resp_cbor_auth_key) != 0) {
        return -1;
    }
    DEBUG_PUTS("[coap_responder]: load and set CBOR RPK");
    if (cred_rpk_from_cbor(rpk, resp_cbor_rpk, sizeof(resp_cbor_rpk) != 0) {
        return -1;
    }
    DEBUG_PUTS("[coap_responder]: load credential identifier information");
    if (cred_id_from_cbor(cred_id, resp_rpk_id, sizeof(resp_rpk_id) != 0) {
        return -1;
    }
    DEBUG_PUTS("[coap_responder]: set up EDHOC callbacks and role");
    edhoc_conf_setup_ad_callbacks(conf, NULL, NULL, NULL);
    if (edhoc_conf_setup_role(conf, EDHOC_IS_RESPONDER) != 0) {
        return -1;
    }
    DEBUG_PUTS("[coap_responder]: set up EDHOC credentials");
    if (edhoc_conf_setup_credentials(conf, auth_key, CRED_TYPE_RPK, rpk, cred_id, _cred_cb) != 0) {
        return -1;
    }
    edhoc_ctx_setup(ctx, conf, hash_ctx);

    return 0;
}
