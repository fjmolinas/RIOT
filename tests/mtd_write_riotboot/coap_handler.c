/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "net/nanocoap.h"
#include "riotboot/slot.h"

#include "mtd.h"
#include "mtd_mapper.h"
#include "mtd_flashpage.h"
#include "mtd_writer.h"

#define MTD_WRITER_BUF_SIZE         512

/* When writing raw bytes on flash, data must be correctly aligned. */
#define ALIGNMENT_ATTR __attribute__((aligned(FLASHPAGE_WRITE_BLOCK_ALIGNMENT)))

static mtd_flashpage_t _dev = MTD_FLASHPAGE_INIT_VAL((FLASHPAGE_SIZE / MTD_WRITER_BUF_SIZE));
static mtd_mapper_parent_t _parent = MTD_PARENT_INIT(&_dev.base);

static mtd_mapper_region_t _slot = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = SLOT0_LEN / FLASHPAGE_SIZE,
        .pages_per_sector = FLASHPAGE_SIZE / MTD_WRITER_BUF_SIZE,
        .page_size = MTD_WRITER_BUF_SIZE,
    },
    .parent = &_parent,
};

static mtd_dev_t *_dev_slot = &_slot.mtd;

static uint8_t ALIGNMENT_ATTR _buf[MTD_WRITER_BUF_SIZE];
static mtd_writer_t _writer;

ssize_t _write_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    mtd_writer_t *writer = context;

    uint32_t result = COAP_CODE_204;

    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);


    printf("_write_handler(): received data: offset=%u len=%u blockwise=%i more=%i\n", \
           (unsigned)block1.offset, pkt->payload_len, blockwise, block1.more);

    uint8_t *payload_start = pkt->payload;
    size_t payload_len = pkt->payload_len;

    if (block1.offset == 0) {
        _slot.sector = flashpage_page((void *)riotboot_slot_get_hdr(riotboot_slot_other())),
        printf("_write_handler(): init len=%u, sector=%" PRIu32 "\n", pkt->payload_len,
               _slot.sector);
        mtd_writer_init(writer, _dev_slot, 0, _buf, sizeof(_buf));
    }


    if (block1.offset == writer->offset) {
        mtd_writer_putbytes(writer, payload_start, payload_len, block1.more);
    }
    else {
        printf("_write_handler(): skipping invalid offset (data=%u, writer=%u)\n",
               (unsigned)block1.offset, (unsigned)writer->offset);
    }

    if (block1.more == 1) {
        result = COAP_CODE_CONTINUE;
    }

    if (!blockwise || !block1.more) {
        printf("_write_handler(): finish %d\n", riotboot_slot_validate(riotboot_slot_other()));
    }

    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    uint8_t *pkt_pos = (uint8_t *)pkt->hdr + reply_len;

    pkt_pos += coap_put_block1_ok(pkt_pos, &block1, 0);

    return pkt_pos - (uint8_t *)pkt->hdr;
}

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/flashwrite", COAP_POST, _write_handler, &_writer },
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);
