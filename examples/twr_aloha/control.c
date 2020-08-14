/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <uwb/uwb.h>
#include <uwb_rng/uwb_rng.h>
#include <dpl/dpl.h>
#include "control.h"

#include "shell_commands.h"
#include "shell.h"

#define LOG_LEVEL LOG_DEBUG
#include "log.h"

#define MYNEWT_VAL_ANCHOR_ADDRESS 0x1234

#include "uwbcfg/uwbcfg.h"

uint8_t ranging_enabled_flag = 0;
struct dpl_callout tx_callout;

#if MYNEWT_VAL(UWBCFG_ENABLED)
static bool g_uwb_config_needs_update = false;
static int uwb_config_updated(void)
{
    g_uwb_config_needs_update = true;
    return 0;
}

static struct uwbcfg_cbs uwb_cb = {
    .uc_update = uwb_config_updated
};
#endif

static void _start_ranging(void)
{
    LOG_DEBUG("Start ranging\n");
    dpl_callout_reset(&tx_callout, DPL_TICKS_PER_SEC/25);
    ranging_enabled_flag = 1;
}
static void _stop_ranging(void)
{
    LOG_DEBUG("Stop ranging\n");
    dpl_callout_stop(&tx_callout);
    ranging_enabled_flag = 0;
}

static int _range_cli_cmd(int argc, char **argv)
{
    (void) argc;

    if (!strcmp(argv[1], "start")) {
        _start_ranging();
    }
    else if (!strcmp(argv[1], "stop")) {
        _stop_ranging();
    }
    else {
        puts("Usage:");
        puts("\trange start:  to start ranging");
        puts("\trange stop:  to stop  ranging");
    }

    return 0;
}


static const shell_command_t shell_commands[] = {
    { "range", "Control command", _range_cli_cmd },
    { NULL, NULL, NULL }
};

/* forward declaration */
static void slot_complete_cb(struct dpl_event * ev);
static bool complete_cb(struct uwb_dev * inst, struct uwb_mac_interface * cbs);
static bool rx_timeout_cb(struct uwb_dev * inst, struct uwb_mac_interface * cbs);

static struct uwb_mac_interface g_cbs = (struct uwb_mac_interface){
    .id = UWBEXT_APP0,
    .complete_cb = complete_cb,
    .rx_timeout_cb = rx_timeout_cb,
};

/**
 * @brief This callback is part of the  struct uwb_mac_interface extension interface and invoked of the completion of a range request
 * in the context of this example. The struct uwb_mac_interface is in the interrupt context and is used to schedule events an event queue.
 * Processing should be kept to a minimum giving the interrupt context. All algorithms activities should be deferred to a thread on an event queue.
 * The callback should return true if and only if it can determine if it is the sole recipient of this event.
 *
 * NOTE: The MAC extension interface is a link-list of callbacks, subsequent callbacks on the list will be not be called in the
 * event of returning true.
 *
 * @param inst  - struct uwb_dev *
 * @param cbs   - struct uwb_mac_interface *
 *
 * output parameters
 *
 * returns bool
 */
/* The timer callout */
static struct dpl_event slot_event = {0};
static uint16_t g_idx_latest;

static bool complete_cb(struct uwb_dev * inst, struct uwb_mac_interface * cbs)
{
    if (inst->fctrl != FCNTL_IEEE_RANGE_16 &&
        inst->fctrl != (FCNTL_IEEE_RANGE_16 | UWB_FCTRL_ACK_REQUESTED)) {
        return false;
    }
    struct uwb_rng_instance* rng = (struct uwb_rng_instance*)cbs->inst_ptr;
    g_idx_latest = (rng->idx)%rng->nframes; // Store valid frame pointer
    dpl_eventq_put(dpl_eventq_dflt_get(), &slot_event);
    return true;
}

/**
 * @brief API for rx_timeout_cb of ccp.
 *
 * @param inst   Pointer to struct uwb_dev.
 * @param cbs    Pointer to struct uwb_mac_interface.
 *
 * @return void
 */
static bool rx_timeout_cb(struct uwb_dev * inst, struct uwb_mac_interface * cbs)
{
    struct uwb_rng_instance * rng = (struct uwb_rng_instance*)cbs->inst_ptr;
    if (inst->role & UWB_ROLE_ANCHOR) {
        uwb_phy_forcetrxoff(inst);
        uwb_rng_listen(rng, 0xfffff, UWB_NONBLOCKING);
    } else {
        /* Do nothing */
    }
    return true;
}

/**
 * @brief In the example this function represents the event context processing of the received range request.
 * In this case, a JSON string is constructed and written to stdio. See the ./apps/matlab or ./apps/python folders for examples on
 * how to parse and render these results.
 *
 * input parameters
 * @param inst - struct dpl_event *
 * output parameters
 * returns none
 */

static void slot_complete_cb(struct dpl_event *ev)
{
    assert(ev != NULL);

    struct uwb_rng_instance * rng = (struct uwb_rng_instance *)dpl_event_get_arg(ev);
    struct uwb_dev * inst = rng->dev_inst;

    if (inst->role & UWB_ROLE_ANCHOR) {
        uwb_rng_listen(rng, 0xfffff, UWB_NONBLOCKING);
    }
}


static void uwb_ev_cb(struct dpl_event *ev)
{
    struct uwb_rng_instance * rng = (struct uwb_rng_instance *)ev->arg;
    struct uwb_dev * inst = rng->dev_inst;

    if (inst->role&UWB_ROLE_ANCHOR) {
        if(dpl_sem_get_count(&rng->sem) == 1){
#if MYNEWT_VAL(UWBCFG_ENABLED)
            if (g_uwb_config_needs_update) {
                uwb_mac_config(inst, NULL);
                uwb_txrf_config(inst, &inst->config.txrf);

                inst->my_short_address = MYNEWT_VAL(ANCHOR_ADDRESS);
                uwb_set_uid(inst, inst->my_short_address);
                g_uwb_config_needs_update = false;
            }
#endif
            uwb_rng_listen(rng, 0xfffff, UWB_NONBLOCKING);
        }
    }
    else {
        int mode_v[8] = {0}, mode_i=0, mode=-1;
        static int last_used_mode = 0;
#if MYNEWT_VAL(UWBCFG_ENABLED)
        if (g_uwb_config_needs_update) {
            uwb_mac_config(inst, NULL);
            uwb_txrf_config(inst, &inst->config.txrf);

            inst->my_short_address = inst->euid&0xffff;
            uwb_set_uid(inst, inst->my_short_address);
            g_uwb_config_needs_update = false;
    }
#endif
#if MYNEWT_VAL(TWR_SS_ENABLED)
        mode_v[mode_i++] = UWB_DATA_CODE_SS_TWR;
#endif
#if MYNEWT_VAL(TWR_SS_ACK_ENABLED)
        mode_v[mode_i++] = UWB_DATA_CODE_SS_TWR_ACK;
#endif
#if MYNEWT_VAL(TWR_SS_EXT_ENABLED)
        mode_v[mode_i++] = UWB_DATA_CODE_SS_TWR_EXT;
#endif
#if MYNEWT_VAL(TWR_DS_ENABLED)
        mode_v[mode_i++] = UWB_DATA_CODE_DS_TWR;
#endif
#if MYNEWT_VAL(TWR_DS_EXT_ENABLED)
        mode_v[mode_i++] = UWB_DATA_CODE_DS_TWR_EXT;
#endif
        if (++last_used_mode >= mode_i) last_used_mode=0;
        mode = mode_v[last_used_mode];
#ifdef UWB_FIXED_TWR_ALGORITHM
        mode = UWB_FIXED_TWR_ALGORITHM;
#endif
        if (mode > 0) {
            uwb_rng_request(rng, MYNEWT_VAL(ANCHOR_ADDRESS), mode);
        }
    }

    if(ranging_enabled_flag) {
        dpl_callout_reset(&tx_callout, DPL_TICKS_PER_SEC/64);
    }
}


void *init_ranging(void * arg)
{
    struct uwb_dev *udev = uwb_dev_idx_lookup(0);
    struct uwb_rng_instance* rng = (struct uwb_rng_instance*)uwb_mac_find_cb_inst_ptr(udev, UWBEXT_RNG);
    assert(rng);

#if MYNEWT_VAL(UWBCFG_ENABLED)
    uwbcfg_register(&uwb_cb);
#endif
    g_cbs.inst_ptr = rng;
    uwb_mac_append_interface(udev, &g_cbs);

    uint32_t utime = dpl_cputime_ticks_to_usecs(dpl_cputime_get32());
    LOG_DEBUG("{\"utime\": %"PRIu32",\"exec\": \"%s\"}\n",utime,__FILE__);
    LOG_DEBUG("{\"device_id\"=\"%"PRIx32"\"",udev->device_id);
    LOG_DEBUG(",\"panid=\"%X\"",udev->pan_id);
    LOG_DEBUG(",\"addr\"=\"%X\"",udev->uid);
    LOG_DEBUG(",\"part_id\"=\"%"PRIx32"\"",(uint32_t)(udev->euid&0xffffffff));
    LOG_DEBUG(",\"lot_id\"=\"%"PRIx32"\"}\n",(uint32_t)(udev->euid>>32));
    LOG_DEBUG("{\"utime\": %lu,\"msg\": \"frame_duration = %d usec\"}\n",utime, uwb_phy_frame_duration(udev, sizeof(twr_frame_final_t)));
    LOG_DEBUG("{\"utime\": %lu,\"msg\": \"SHR_duration = %d usec\"}\n",utime,uwb_phy_SHR_duration(udev));
    LOG_DEBUG("{\"utime\": %lu,\"msg\": \"holdoff = %d usec\"}\n",utime,(uint16_t)ceilf(uwb_dwt_usecs_to_usecs(rng->config.tx_holdoff_delay)));

#if MYNEWT_VAL(TWR_SS_ACK_ENABLED)
    uwb_set_autoack(udev, true);
    uwb_set_autoack_delay(udev, 12);
#endif

    dpl_callout_init(&tx_callout, dpl_eventq_dflt_get(), uwb_ev_cb, rng);
    dpl_callout_reset(&tx_callout, DPL_TICKS_PER_SEC/25);

    dpl_event_init(&slot_event, slot_complete_cb, rng);

    /* Apply config */
    uwb_mac_config(udev, NULL);
    uwb_txrf_config(udev, &udev->config.txrf);

    if ((udev->role & UWB_ROLE_ANCHOR)) {
        LOG_DEBUG("Node role: ANCHOR \n");
        udev->my_short_address = MYNEWT_VAL(ANCHOR_ADDRESS);
        uwb_set_uid(udev, udev->my_short_address);
    } else{
        LOG_DEBUG("Node role: TAG \n");
    }

#if MYNEWT_VAL(RNG_VERBOSE) > 1
    udev->config.rxdiag_enable = 1;
#else
    udev->config.rxdiag_enable = 0;
#endif

    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return arg;
}
