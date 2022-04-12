/*
 * Copyright (C) 2020 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit_storage
 * @{
 *
 * @file
 * @brief       SUIT storage backend helpers
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>
#include "kernel_defines.h"

#include "suit.h"
#include "suit/storage.h"

#include "irq.h"
#include "xfa.h"

XFA_INIT(suit_storage_t *, suit_storage_reg);

suit_storage_t *suit_storage_find_by_id(const char *id)
{
    for (size_t i = 0; i < XFA_LEN(suit_storage_t *, suit_storage_reg); i++) {
        if (suit_storage_has_location(suit_storage_reg[i], id)) {
            return suit_storage_reg[i];
        }
    }
    return NULL;
}

void suit_storage_init_all(void)
{
    for (size_t i = 0; i < XFA_LEN(suit_storage_t *, suit_storage_reg); i++) {
        suit_storage_init(suit_storage_reg[i]);
    }
}

suit_storage_t *suit_storage_find_by_component(const suit_manifest_t *manifest,
                                               const suit_component_t *component)
{
    for (size_t i = 0; i < XFA_LEN(suit_storage_t *, suit_storage_reg); i++) {
        char name[CONFIG_SUIT_COMPONENT_MAX_NAME_LEN];
        if (suit_component_name_to_string(manifest, component,
                                          suit_storage_reg[i]->driver->separator,
                                          name, sizeof(name)) == SUIT_OK) {

            if (suit_storage_has_location(suit_storage_reg[i], name)) {
                return suit_storage_reg[i];
            }
        }
    }
    return NULL;
}

int suit_storage_get_highest_seq_no(uint32_t *seq_no)
{
    uint32_t max_seq = 0;
    int res = SUIT_ERR_STORAGE;

    for (size_t i = 0; i < XFA_LEN(suit_storage_t *, suit_storage_reg); i++) {
        uint32_t seq_no = 0;
        if (suit_storage_get_seq_no(suit_storage_reg[i], &seq_no) == SUIT_OK) {
            res = SUIT_OK;
            if (seq_no > max_seq) {
                max_seq = seq_no;
            }
        }
    }
    *seq_no = max_seq;
    return res;
}

int suit_storage_set_seq_no_all(uint32_t seq_no)
{
    for (size_t i = 0; i < XFA_LEN(suit_storage_t *, suit_storage_reg); i++) {
        suit_storage_set_seq_no(suit_storage_reg[i], seq_no);
    }
    return 0;
}

static int _suit_storage_add_hook(suit_storage_t *storage, suit_storage_hook_t *hook, bool pre)
{
    suit_storage_hook_t *head = pre ? storage->pre : storage->post;
    unsigned state = irq_disable();

    hook->next = head;
    head = hook;
    irq_restore(state);
    return SUIT_OK;
}

static int _suit_storage_rmv_hook(suit_storage_t *storage, suit_storage_hook_t *hook, bool pre)
{
    unsigned state = irq_disable();
    suit_storage_hook_t **list = pre ? &storage->pre : &storage->post;

    while (*list) {
        if (*list == hook) {
            *list = hook->next;
            irq_restore(state);
            return -1;
        }
        list = &(*list)->next;
    }
    irq_restore(state);
    return SUIT_OK;
}

int suit_storage_add_pre_hook(suit_storage_t *storage, suit_storage_hook_t *hook)
{
    return _suit_storage_add_hook(storage, hook, true);
}


int suit_storage_add_post_hook(suit_storage_t *storage, suit_storage_hook_t *hook)
{
    return _suit_storage_add_hook(storage, hook, false);
}

int suit_storage_rmv_pre_hook(suit_storage_t *storage, suit_storage_hook_t *hook)
{
    return _suit_storage_rmv_hook(storage, hook, true);
}

int suit_storage_rmv_post_hook(suit_storage_t *storage, suit_storage_hook_t *hook)
{
    return _suit_storage_rmv_hook(storage, hook, false);
}
