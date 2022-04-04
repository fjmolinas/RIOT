/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       SUIT updates over CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>
#include <fcntl.h>

#include "fmt.h"
#include "thread.h"
#include "irq.h"
#include "net/nanocoap_sock.h"
#include "board.h"

#include "shell.h"

#include "suit/transport/coap.h"
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
#include "riotboot/slot.h"
#endif

#include "xfa.h"
#include "suit/storage.h"
#include "suit/storage/ram.h"
#include "suit/storage/mtd.h"
#include "suit/storage/vfs.h"

#ifdef MODULE_PERIPH_GPIO
#include "periph/gpio.h"
#endif

#define COAP_INBUF_SIZE (256U)

/* Extend stacksize of nanocoap server thread */
static char _nanocoap_server_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#define NANOCOAP_SERVER_QUEUE_SIZE     (8)
static msg_t _nanocoap_server_msg_queue[NANOCOAP_SERVER_QUEUE_SIZE];

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#if !defined(MTD_0) && MODULE_MTD_SDCARD
#include "mtd_sdcard.h"
#include "sdcard_spi.h"
#include "sdcard_spi_params.h"

#define SDCARD_SPI_NUM ARRAY_SIZE(sdcard_spi_params)

/* SD card devices are provided by drivers/sdcard_spi/sdcard_spi.c */
extern sdcard_spi_t sdcard_spi_devs[SDCARD_SPI_NUM];

/* Configure MTD device for the first SD card */
static mtd_sdcard_t mtd_sdcard_dev = {
    .base = {
        .driver = &mtd_sdcard_driver
    },
    .sd_card = &sdcard_spi_devs[0],
    .params = &sdcard_spi_params[0],
};
static mtd_dev_t *mtd_sdcard = (mtd_dev_t*)&mtd_sdcard_dev;
#define MTD_0 mtd_sdcard
#endif

// #if IS_USED(MODULE_SUIT_STORAGE_MTD) && defined(MTD_0)
// XFA_USE(suit_storage_mtd_region_t, suit_storage_mtd_reg);
// XFA(suit_storage_mtd_reg, 0) suit_storage_mtd_region_t suit_storage_mtd_0 = {
//     .used = 0,
//     .dev = NULL
// };
// #endif

// #if IS_USED(MODULE_SUIT_STORAGE_VFS) && defined(MTD_0)
#include "fs/fatfs.h"
static fatfs_desc_t fs_desc;
static vfs_mount_t mnt = {
    .mount_point = "/sda",
    .fs = &fatfs_file_system,
    .private_data = &fs_desc,
};
extern suit_storage_vfs_t suit_storage_vfs;

XFA_USE(char*, suit_storage_files_reg);
XFA(suit_storage_files_reg, 0) char* _slot0_file = "/sda/SLOT0.txt";
XFA(suit_storage_files_reg, 0) char* _slot1_file = "/sda/SLOT1.txt";
// #endif

static void *_nanocoap_server_thread(void *arg)
{
    (void)arg;

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_nanocoap_server_msg_queue, NANOCOAP_SERVER_QUEUE_SIZE);

    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port = COAP_PORT, .family = AF_INET6 };

    nanocoap_server(&local, buf, sizeof(buf));

    return NULL;
}

/* assuming that first button is always BTN0 */
#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
static void cb(void *arg)
{
    (void)arg;
    printf("Button pressed! Triggering suit update! \n");
    suit_coap_trigger((uint8_t *)SUIT_MANIFEST_RESOURCE, sizeof(SUIT_MANIFEST_RESOURCE));
}
#endif

#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
static int cmd_print_riotboot_hdr(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int current_slot = riotboot_slot_current();

    if (current_slot != -1) {
        /* Sometimes, udhcp output messes up the following printfs.  That
         * confuses the test script. As a workaround, just disable interrupts
         * for a while.
         */
        unsigned state = irq_disable();
        riotboot_slot_print_hdr(current_slot);
        irq_restore(state);
    }
    else {
        printf("[FAILED] You're not running riotboot\n");
    }
    return 0;
}

static int cmd_print_current_slot(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    /* Sometimes, udhcp output messes up the following printfs.  That
     * confuses the test script. As a workaround, just disable interrupts
     * for a while.
     */
    unsigned state = irq_disable();

    printf("Running from slot %d\n", riotboot_slot_current());
    irq_restore(state);
    return 0;
}
#endif

static int cmd_print_slot_content(int argc, char **argv)
{
    char *slot;
    uint32_t offset;
    size_t len;

    if (argc < 4) {
        printf("usage: %s <storage_id> <addr> <len>\n", argv[0]);
        return -1;
    }

    slot = argv[1];
    offset = atoi(argv[2]);
    len = atoi(argv[3]);

    suit_storage_t *storage = suit_storage_find_by_id(slot);

    if (!storage) {
        printf("No storage with id \"%s\" present\n", slot);
        return -1;
    }
    else {
        puts("found location");
    }

    suit_storage_set_active_location(storage, slot);

    if (suit_storage_has_readptr(storage)) {
        const uint8_t *buf;
        size_t available;
        suit_storage_read_ptr(storage, &buf, &available);

        size_t to_print = available < offset + len ? available - offset : len;
        for (size_t i = offset; i < to_print; i++) {
            print_byte_hex(buf[i]);
        }
        puts("");
    }

    int fd = vfs_open(slot, O_RDONLY, 0);
    if (fd < 0) {
        return -1;
    }
    char c;
    while (vfs_read(fd, &c, 1) != 0) {
        putchar(c);
    }
    vfs_close(fd);
    return 0;
}

static int cmd_lsstorage(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (IS_ACTIVE(MODULE_SUIT_STORAGE_RAM)) {
        for (unsigned i = 0; i < CONFIG_SUIT_STORAGE_RAM_REGIONS; i++) {
            printf("RAM slot %u: \"%s%u\"\n", i,
                   CONFIG_SUIT_STORAGE_RAM_LOCATION_PREFIX, i);
        }
    }
    if (IS_ACTIVE(MODULE_SUIT_STORAGE_FLASHWRITE)) {
        puts("Flashwrite slot 0: \"\"\n");
    }
#if IS_ACTIVE(MODULE_SUIT_STORAGE_MTD) && defined(MTD_0)
        printf("MTD slot %u: \"%s%u\"\n", 0,
                CONFIG_SUIT_STORAGE_MTD_LOCATION_PREFIX, 0);
#endif

    return 0;
}

static const shell_command_t shell_commands[] = {
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
    { "current-slot", "Print current slot number", cmd_print_current_slot },
    { "riotboot-hdr", "Print current slot header", cmd_print_riotboot_hdr },
#endif
    { "storage_content", "Print the slot content", cmd_print_slot_content },
    { "lsstorage", "Print the available storage paths", cmd_lsstorage },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT SUIT update example application");

    /* initialize storage units */
// #if IS_USED(MODULE_SUIT_STORAGE_MTD) && defined(MTD_0)
//     suit_storage_mtd_0.dev = MTD_0;
// #endif

// #if IS_USED(MODULE_SUIT_STORAGE_VFS) && defined(MTD_0)
    fs_desc.dev = MTD_0;
    suit_storage_vfs.mnt = &mnt;
#ifdef BOARD_NATIVE
    if(vfs_mount(&mnt) < 0) {
        vfs_format(&mnt);
    }
#endif
// #endif
    suit_storage_init_all();

// #ifdef BOARD_NATIVE
    ipv6_addr_t addr;
    const char addr_str[] = "2001:db8::2";
    ipv6_addr_from_str(&addr, addr_str);
    gnrc_netif_ipv6_addr_add(gnrc_netif_iter(NULL), &addr, 64, 0);
// #endif

#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
    /* initialize a button to manually trigger an update */
    gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_FALLING, cb, NULL);
#endif

#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
    cmd_print_current_slot(0, NULL);
    cmd_print_riotboot_hdr(0, NULL);
#endif

    /* start suit coap updater thread */
    suit_coap_run();

    /* start nanocoap server thread */
    thread_create(_nanocoap_server_stack, sizeof(_nanocoap_server_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  _nanocoap_server_thread, NULL, "nanocoap server");

    /* the shell contains commands that receive packets via GNRC and thus
       needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Starting the shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
