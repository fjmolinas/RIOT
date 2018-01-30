/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Demonstrating the sending and receiving of UDP data
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#include "timex.h"
#include "xtimer.h"

static gnrc_netreg_entry_t server = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                               KERNEL_PID_UNDEF);

/**
* @brief   UDP server variables
*/
kernel_pid_t server_thread_pid = KERNEL_PID_UNDEF;

#define SERVER_MSG_QUEUE_SIZE   (16)

static char server_stack[THREAD_STACKSIZE_DEFAULT];

static source_port;
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
static unit8_t packet_data = []
xtimer_t timer;

static int server_echo(gnrc_pktsnip_t *pkt)
{
    udp_hdr_t *hdr = pkt->data;
    ipv6_hdr_t *hdr = pkt->data;

    uint8_t source_port = 0x00;

    switch (pkt->type) {
        case GNRC_NETTYPE_UNDEF:
            printf("NETTYPE_UNDEF (%i)\n", pkt->type);
            od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
            break;
        case GNRC_NETTYPE_NETIF:
            printf("NETTYPE_NETIF (%i)\n", pkt->type);
            gnrc_pktbuf_release(pkt);
            break;
        case GNRC_NETTYPE_SIXLOWPAN:
            printf("NETTYPE_SIXLOWPAN (%i)\n", pkt->type);
            gnrc_pktbuf_release(pkt);
            break;
        case GNRC_NETTYPE_IPV6:
            printf("NETTYPE_IPV6 (%i)\n", pkt->type);
            ipv6_addr_to_str(addr_str, &hdr->dst, sizeof(addr_str));
            gnrc_pktbuf_release(pkt);
            break;
        case GNRC_NETTYPE_ICMPV6:
            printf("NETTYPE_ICMPV6 (%i)\n", pkt->type);
            gnrc_pktbuf_release(pkt);
            break;
        case GNRC_NETTYPE_UDP:
            printf("NETTYPE_UDP (%i)\n", pkt->type);
            source_port = byteorder_ntohs(hdr->src_port);
            gnrc_pktbuf_release(pkt);
            break;
        default:
            printf("NETTYPE_UNKNOWN (%i)\n", pkt->type);
            od_hex_dump(pkt->data, pkt->size, OD_WIDTH_DEFAULT);
            break;
    }

}

static void *_server_thread(void *args)
{

    (void) args;
    msg_t msg, reply;
    msg_t msg_queue[SERVER_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, SERVER_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    gnrc_pktsnip_t *pkt = NULL;

    while (1)
    {
        msg_receive(&msg);

        switch (msg.type)
        {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                puts("UDP_SERVER: data received:");
                pkt = msg.content.ptr;
                server_echo(pkt);
                // xtimer_set_msg(&timer, 1U*US_PER_SEC, msg, server_thread_pid)
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                puts("UDP_SERVER: data to send:");
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                puts("UDP_SERVER: reply sent:");
                break;
            case DELAYED_ECHO:
                puts("UDP_SERVER: echo reply sent:");
                pkt = msg.content.ptr;
                server_echo(pkt);
                break;
            default:
                puts("UDP_SERVER: received something unexpected");
                break;
        }
        return NULL;
    }
}

static void send(char *addr_str, char *port_str, char *data, unsigned int num,
                 unsigned int delay)
{
    uint16_t port;
    ipv6_addr_t addr;

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return;
    }
    /* parse port */
    port = atoi(port_str);
    if (port == 0) {
        puts("Error: unable to parse destination port");
        return;
    }

    for (unsigned int i = 0; i < num; i++) {
        gnrc_pktsnip_t *payload, *udp, *ip;
        unsigned payload_size;
        /* allocate payload */
        payload = gnrc_pktbuf_add(NULL, data, strlen(data), GNRC_NETTYPE_UNDEF);
        if (payload == NULL) {
            puts("Error: unable to copy data to packet buffer");
            return;
        }
        /* store size for output */
        payload_size = (unsigned)payload->size;
        /* allocate UDP header, set source port := destination port */
        udp = gnrc_udp_hdr_build(payload, port, port);
        if (udp == NULL) {
            puts("Error: unable to allocate UDP header");
            gnrc_pktbuf_release(payload);
            return;
        }
        /* allocate IPv6 header */
        ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);
        if (ip == NULL) {
            puts("Error: unable to allocate IPv6 header");
            gnrc_pktbuf_release(udp);
            return;
        }
        /* send packet */
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
            puts("Error: unable to locate UDP thread");
            gnrc_pktbuf_release(ip);
            return;
        }
        /* access to `payload` was implicitly given up with the send operation above
         * => use temporary variable for output */
        printf("Success: sent %u byte(s) to [%s]:%u\n", payload_size, addr_str,
               port);
        xtimer_usleep(delay);
    }
}

static void start_server(char *port_str)
{
    uint16_t port;

    /* check if server is already running */
    if (server.target.pid != KERNEL_PID_UNDEF) {
        printf("Error: server already running on port %" PRIu32 "\n",
               server.demux_ctx);
        return;
    }
    /* parse port */
    port = atoi(port_str);
    if (port == 0) {
        puts("Error: invalid port specified");
        return;
    }
    /* start server (which means registering pktdump for the chosen port) */
    server.target.pid = server_thread_pid;
    server.demux_ctx = (uint32_t)port;
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &server);
    printf("Success: started UDP server on port %" PRIu16 "\n", port);
}

 void create_server(void)
{
    if (server_thread_pid == KERNEL_PID_UNDEF) {
        server_thread_pid = thread_create(server_stack, sizeof(server_stack), 6,
                             THREAD_CREATE_STACKTEST,
                             _server_thread, NULL, "pktdump");
    }
    return;
}

static void stop_server(void)
{
    /* check if server is running at all */
    if (server.target.pid == KERNEL_PID_UNDEF) {
        printf("Error: server was not running\n");
        return;
    }
    /* stop server */
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &server);
    server.target.pid = KERNEL_PID_UNDEF;
    puts("Success: stopped UDP server");
}

int udp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000;
        if (argc < 5) {
            printf("usage: %s send <addr> <port> <data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 5) {
            num = atoi(argv[5]);
        }
        if (argc > 6) {
            delay = atoi(argv[6]);
        }
        send(argv[2], argv[3], argv[4], num, delay);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <port>\n", argv[0]);
                return 1;
            }
            start_server(argv[3]);
        }
        else if (strcmp(argv[2], "stop") == 0) {
            stop_server();
        }
        else {
            puts("error: invalid command");
        }
    }
    else {
        puts("error: invalid command");
    }
    return 0;
}
