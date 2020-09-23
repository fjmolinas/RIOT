/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dw1000
 * @{
 *
 * @file
 * @brief       Device driver implementation for the driver_dw1000
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "iolist.h"
#include "xtimer.h"

#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "dw1000.h"
#include "dw1000_hal.h"
#include "dw1000_netdev.h"
#include "libdw1000.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif
#include "log.h"

static int _send(netdev_t *netdev, const iolist_t *iolist);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static void _isr(netdev_t *netdev);
static int _init(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len);

const netdev_driver_t dw1000_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _irq_handler(void *arg)
{
    netdev_trigger_event_isr(arg);
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    (void) netdev;
    (void) iolist;
    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void) netdev;
    (void) buf;
    (void) len;
    (void) info;
    return 0;
}

static int _init(netdev_t *netdev)
{
    dw1000_t *dev = (dw1000_t *)netdev;

 /* set semaphore */
    dpl_sem_init(params->spi_sem, 0x1);
    /* set default uwb config */
    memcpy(&dev->inst, &dw1000_instance_config_default,
           sizeof(dw1000_dev_instance_t));

    /* udev initiation stuff */
    uwb_dev_init(udev);

    err = dpl_mutex_init(&inst->mutex);
    err = dpl_sem_init(&inst->tx_sem, 0x1);
    err = dpl_sem_init(&inst->spi_nb_sem, 0x1);

    // dw1000_dev_config(dev);

    int rc;
    int timeout = 3;
retry:
    inst->spi_settings.baudrate = inst->spi_baudrate_low;
    hal_dw1000_reset(inst);
    rc = hal_spi_disable(inst->spi_num);
    assert(rc == 0);
    rc = hal_spi_config(inst->spi_num, &inst->spi_settings);
    assert(rc == 0);
    rc = hal_spi_enable(inst->spi_num);
    assert(rc == 0);

    inst->uwb_dev.device_id = dw1000_read_reg(inst, DEV_ID_ID, 0, sizeof(uint32_t));
    inst->uwb_dev.status.initialized = (inst->uwb_dev.device_id == DWT_DEVICE_ID);
    if (!inst->uwb_dev.status.initialized && --timeout)
    {
        /* In case dw1000 was sleeping */
        dw1000_dev_wakeup(inst);
        goto retry;
    }

    /* init phy */
    dw1000_phy_init(inst, NULL);
    /* It's now safe to increase the SPI baudrate > 4M */
    inst->spi_settings.baudrate = inst->spi_baudrate;
    rc = hal_spi_disable(inst->spi_num);
    assert(rc == 0);
    rc = hal_spi_config(inst->spi_num, &inst->spi_settings);
    assert(rc == 0);
    rc = hal_spi_enable(inst->spi_num);
    assert(rc == 0);

    /* Configure DW1000 */
    dw1000_mac_config(inst, config);

    /* */
    dw1000_set_panid(inst, inst->uwb_dev.pan_id);
    dw1000_set_eui(inst, inst->uwb_dev.euid);
    dw1000_set_address16(i nst,inst->uwb_dev.uid);

    /* setup interrupts */
    /* Enable pull-down on IRQ to not get spurious interrupts when dw1000 is sleeping */
    hal_gpio_irq_init(inst->irq_pin, dw1000_irq, inst, HAL_GPIO_TRIG_RISING, HAL_GPIO_PULL_DOWN);
    hal_gpio_irq_enable(inst->irq_pin);
    /* Setup interrupt mask */
    dw1000_phy_interrupt_mask(inst,          SYS_MASK_MCPLOCK | SYS_MASK_MRXDFR | SYS_MASK_MLDEERR | SYS_MASK_MTXFRB | SYS_MASK_MTXFRS | SYS_MASK_ALL_RX_TO   | SYS_MASK_ALL_RX_ERR | SYS_MASK_MTXBERR, false);
    dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_SLP2INIT | SYS_STATUS_CPLOCK| SYS_STATUS_RXDFR | SYS_STATUS_LDEERR | SYS_STATUS_TXFRB | SYS_STATUS_TXFRS | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR | SYS_STATUS_TXBERR, sizeof(uint32_t));
    dw1000_phy_interrupt_mask(inst,          SYS_MASK_MCPLOCK | SYS_MASK_MRXDFR | SYS_MASK_MLDEERR | SYS_MASK_MTXFRB | SYS_MASK_MTXFRS | SYS_MASK_ALL_RX_TO   | SYS_MASK_ALL_RX_ERR | SYS_MASK_MTXBERR, true);


    return 0;
}


static void _isr(netdev_t *netdev)
{
    (void) netdev;
        uint16_t finfo;
    struct uwb_mac_interface * cbs = NULL;
    dw1000_dev_instance_t * inst = dpl_event_get_arg(ev);
    dpl_error_t err = dpl_sem_pend(&inst->uwb_dev.irq_sem,  DPL_TIMEOUT_NEVER);
    if (err != DPL_OK) {
        inst->uwb_dev.status.sem_error = 1;
        goto sem_error_exit;
    }

    /* Read status register */
#if MYNEWT_VAL(DW1000_SYS_STATUS_BACKTRACE_LEN)
    {
        uint32_t irq_utime = dpl_cputime_get32();
#endif
        inst->sys_status = dw1000_read_reg(inst, SYS_STATUS_ID, 0, sizeof(uint32_t));
        /* Check for higher status bits only if needed */
        if (!(inst->sys_status & (SYS_MASK_MCPLOCK | SYS_MASK_MRXDFR | SYS_MASK_MLDEERR | SYS_MASK_MTXFRB | SYS_MASK_MTXFRS | SYS_MASK_ALL_RX_TO | SYS_MASK_ALL_RX_ERR | SYS_MASK_MTXBERR))) {
            inst->sys_status_hi = dw1000_read_reg(inst, SYS_STATUS_ID, 4, sizeof(uint8_t));
        }

#if MYNEWT_VAL(DW1000_SYS_STATUS_BACKTRACE_LEN)
        if(!inst->sys_status_bt_lock) {
            DW1000_SYS_STATUS_BT_ADD(inst, inst->sys_status, irq_utime);
#if MYNEWT_VAL(DW1000_SYS_STATUS_BACKTRACE_HI)
            DW1000_SYS_STATUS_BT_HI(inst, inst->sys_status_hi);
#endif
        }
    }
#endif

    // Set status flags
    inst->uwb_dev.status.rx_error = (inst->sys_status & SYS_STATUS_ALL_RX_ERR) !=0;
    inst->uwb_dev.status.rx_error |= (inst->sys_status_hi & (SYS_STATUS_RXRSCS>>32)) != 0;
    inst->uwb_dev.status.rx_autoframefilt_rej = (inst->sys_status & SYS_STATUS_AFFREJ) !=0;
    inst->uwb_dev.status.rx_timeout_error = (inst->sys_status & SYS_STATUS_ALL_RX_TO) !=0;
    inst->uwb_dev.status.lde_error = (inst->sys_status & SYS_STATUS_LDEDONE) == 0;
    inst->uwb_dev.status.overrun_error = (inst->sys_status & SYS_STATUS_RXOVRR) != 0;
    inst->uwb_dev.status.txbuf_error = (inst->sys_status & SYS_STATUS_TXBERR) != 0;
    inst->uwb_dev.status.autoack_triggered = (inst->sys_status & SYS_STATUS_AAT) != 0;
    inst->uwb_dev.status.rx_prej = (inst->sys_status_hi & (SYS_STATUS_RXPREJ>>32)) != 0;

    /* Clear tx_sem unless this is a TXFRB and not TXFRS */
    if(dpl_sem_get_count(&inst->tx_sem) == 0 && !(
           (inst->sys_status & SYS_STATUS_TXFRB) != 0 &&
           (inst->sys_status & SYS_STATUS_TXFRS) == 0
           )) {
        dpl_error_t err = dpl_sem_release(&inst->tx_sem);
        assert(err == DPL_OK);
    }

    // leading edge detection complete
    if((inst->sys_status & SYS_STATUS_RXFCG)){
        MAC_STATS_INC(DFR_cnt);

        if (inst->uwb_dev.status.overrun_error){
            MAC_STATS_INC(ROV_err);
            /* Overrun flag has been set */
            dw1000_write_reg(inst, SYS_STATUS_ID, 0, (SYS_STATUS_RXOVRR |SYS_STATUS_LDEDONE | SYS_STATUS_RXDFR | SYS_STATUS_RXFCG | SYS_STATUS_RXFCE | SYS_STATUS_RXDFR), sizeof(uint32_t));
            dw1000_phy_forcetrxoff(inst);
            dw1000_phy_rx_reset(inst);
            dw1000_sync_rxbufptrs(inst);
            dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET+1, SYS_CTRL_RXENAB>>8, sizeof(uint8_t));
            goto early_exit;
        }

        // The DW1000 has a bug that render the hardware auto_enable feature useless when used in conjunction with the double buffering.
        // Consequently, we reenable the transeiver in the MAC-layer as early as possable. Note: The default behavior of MAC-Layer
        // is that the transceiver only returns to the IDLE state with a timeout event occured. The MAC-layer should otherwise reenable.

        if (inst->uwb_dev.config.rxauto_enable == 0 && inst->uwb_dev.config.dblbuffon_enabled) {
            if (inst->control.rxauto_disable == false && !inst->uwb_dev.status.autoack_triggered) {
                dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET+1, SYS_CTRL_RXENAB>>8, sizeof(uint8_t));
                inst->uwb_dev.status.rx_restarted = 1;
            }
            inst->control.rxauto_disable = false;
        }

        /* Read frame info - Only the first two bytes of the register are used here. */
        finfo = dw1000_read_reg(inst, RX_FINFO_ID, RX_FINFO_OFFSET, sizeof(uint16_t));
        /* Report frame length - Standard frame length up to 127,
         * extended frame length up to 1023 bytes */
        inst->uwb_dev.frame_len = (finfo & RX_FINFO_RXFL_MASK_1023);

        /* Remove the two appended CRC bytes from frame if data is present */
        if (inst->uwb_dev.frame_len) inst->uwb_dev.frame_len -= 2;

        /* Read the whole frame */
        dw1000_read_rx(inst, inst->uwb_dev.rxbuf, 0,
                       (inst->uwb_dev.frame_len < inst->uwb_dev.rxbuf_size) ?
                       inst->uwb_dev.frame_len : inst->uwb_dev.rxbuf_size);

        /* First two bytes are frame ctrl */
        inst->uwb_dev.fctrl = ((uint16_t)inst->uwb_dev.rxbuf[1]<<8) | inst->uwb_dev.rxbuf[0];

#if MYNEWT_VAL(DW1000_SYS_STATUS_BACKTRACE_LEN)
        if(!inst->sys_status_bt_lock) {
            DW1000_SYS_STATUS_BT_FCTRL(inst, inst->uwb_dev.fctrl);
        }
#endif

        if (inst->uwb_dev.status.lde_error) // retest lde_error condition
            inst->uwb_dev.status.lde_error = (dw1000_read_reg(inst, SYS_STATUS_ID, 1, sizeof(uint8_t))  & (SYS_STATUS_LDEDONE >> 8)) == 0;
        if (inst->uwb_dev.status.lde_error) // LDE error or LDE late
            MAC_STATS_INC(LDE_err);

        inst->uwb_dev.rxtimestamp = dw1000_read_rxtime(inst);
        if (inst->control.abs_timeout) {
            update_rx_window_timeout(inst, inst->uwb_dev.rxtimestamp);
        }

        if (inst->uwb_dev.status.autoack_triggered) {
            /* Because of a previous frame not being received properly, AAT bit can be set upon the proper reception of a frame not requesting for
             * acknowledgement (ACK frame is not actually sent though). If the AAT bit is set, check ACK request bit in frame control to confirm (this
             * implementation works only for IEEE802.15.4-2011 compliant frames).
             * This issue is not documented at the time of writing this code. It should be in next release of DW1000 User Manual (v2.09, from July 2016). */
            if ((inst->uwb_dev.fctrl & UWB_FCTRL_ACK_REQUESTED) == 0){
                /* Clear AAT status bit in callback data register copy and status */
                dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_AAT, sizeof(uint8_t));
                inst->sys_status &= ~SYS_STATUS_AAT;
                inst->uwb_dev.status.autoack_triggered = 0;
            } else {
                /* Clear RX flags in sys_status */
                dw1000_write_reg(inst, SYS_STATUS_ID, 1, (inst->sys_status&(SYS_STATUS_LDEDONE | SYS_STATUS_RXDFR | SYS_STATUS_RXFCG | SYS_STATUS_RXFCE | SYS_STATUS_RXDFR))>>8, sizeof(uint8_t));
            }
        }

        // Collect RX Frame Quality diagnositics
        if(inst->uwb_dev.config.rxdiag_enable)
            dw1000_read_rxdiag(inst, &inst->rxdiag);

        // Toggle the Host side Receive Buffer Pointer
        if (inst->uwb_dev.config.dblbuffon_enabled) {
            // The rxttcko is a poor replacement for the carrier_integrator but
            // better than nothing
            if (inst->uwb_dev.config.rxttcko_enable) {
                inst->uwb_dev.rxttcko = dw1000_read_time_tracking_offset(inst);
            }

            inst->uwb_dev.status.overrun_error = dw1000_checkoverrun(inst);
            if (inst->uwb_dev.status.overrun_error == 0) {
                /* Check where the receiver is at, and if it's in the same buffer as we are,
                 * mask out interrupt flags to avoid spurious interrupts when clearing status bits */
                if (inst->uwb_dev.config.rxauto_enable) {
                    if (dw1000_ic_and_host_ptrs_equal(inst)) {
                        uint8_t mask = dw1000_read_reg(inst, SYS_MASK_ID, 1 , sizeof(uint8_t));
                        dw1000_write_reg(inst, SYS_MASK_ID, 1, 0, sizeof(uint8_t));
                        dw1000_write_reg(inst, SYS_STATUS_ID, 1, (inst->sys_status&(SYS_STATUS_LDEDONE | SYS_STATUS_RXDFR | SYS_STATUS_RXFCG | SYS_STATUS_RXFCE | SYS_STATUS_RXDFR))>>8, sizeof(uint8_t));
                        dw1000_write_reg(inst, SYS_MASK_ID, 1, mask, sizeof(uint8_t));
                    } else {
                        dw1000_write_reg(inst, SYS_STATUS_ID, 1, (inst->sys_status&(SYS_STATUS_LDEDONE | SYS_STATUS_RXDFR | SYS_STATUS_RXFCG | SYS_STATUS_RXFCE | SYS_STATUS_RXDFR))>>8, sizeof(uint8_t));
                    }
                }
                /* Swap buffers */
                dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET , 0b1, sizeof(uint8_t));
            }else{
                MAC_STATS_INC(ROV_err);
                /* Overrun flag has been set, reset receiver and realign buffers */
                dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_RXOVRR, sizeof(uint32_t));
                dw1000_phy_forcetrxoff(inst);
                dw1000_phy_rx_reset(inst);
                dw1000_sync_rxbufptrs(inst);
                dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET+1, SYS_CTRL_RXENAB>>8, sizeof(uint8_t));
            }
        }else{
            // carrier_integrator only avilable while in single buffer mode.
            inst->uwb_dev.carrier_integrator = dw1000_read_carrier_integrator(inst);
#if MYNEWT_VAL(CIR_ENABLED)
            // Call CIR complete calbacks if present
            if(inst->uwb_dev.config.cir_enable || inst->control.cir_enable) {
                if(!(SLIST_EMPTY(&inst->uwb_dev.interface_cbs))) {
                    SLIST_FOREACH(cbs, &inst->uwb_dev.interface_cbs, next) {
                        if (cbs != NULL && cbs->cir_complete_cb) {
                            if(cbs->cir_complete_cb((struct uwb_dev*)inst,cbs)) continue;
                        }
                    }
                }
                inst->control.cir_enable = false;
            }
#endif
            dw1000_write_reg(inst, SYS_STATUS_ID, 0,
                             inst->sys_status & (SYS_STATUS_LDEDONE | SYS_STATUS_RXPHD | SYS_STATUS_RXDFR |
                                                 SYS_STATUS_RXFCG | SYS_STATUS_RXFCE | SYS_STATUS_RXDFR),
                             sizeof(uint16_t));
            if (inst->control.rxauto_disable == false){
                dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET+1, SYS_CTRL_RXENAB>>8, sizeof(uint8_t));
                inst->uwb_dev.status.rx_restarted = 1;
            }
            inst->control.rxauto_disable = false;

        }

        // Call the corresponding frame services callback if present
        if(!(SLIST_EMPTY(&inst->uwb_dev.interface_cbs))){
            SLIST_FOREACH(cbs, &inst->uwb_dev.interface_cbs, next){
            if (cbs != NULL && cbs->rx_complete_cb)
                if(cbs->rx_complete_cb((struct uwb_dev*)inst,cbs)) continue;
            }
        }
    }

    // Handle TX Frame Begins
    if(inst->sys_status & SYS_STATUS_TXFRB) {
        // Call the corresponding callback if present
        dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_TXFRB, sizeof(uint8_t)); // Clear TX Frame Begins

        if(!(SLIST_EMPTY(&inst->uwb_dev.interface_cbs))){
            SLIST_FOREACH(cbs, &inst->uwb_dev.interface_cbs, next){
            if (cbs!=NULL && cbs->tx_begins_cb)
                if(cbs->tx_begins_cb((struct uwb_dev*)inst,cbs)) break;
            }
        }
    }

    // Handle TX confirmation event
    if(inst->sys_status & SYS_STATUS_TXFRS) {
        MAC_STATS_INC(TFG_cnt);

        dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_ALL_TX, sizeof(uint8_t)); // Clear TX event bits

        if (inst->control.abs_timeout) {
            dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET+1, SYS_CTRL_RXENAB>>8, sizeof(uint8_t));
            update_rx_window_timeout(inst, dw1000_read_txtime(inst));
        }

        if(dpl_sem_get_count(&inst->tx_sem) == 0){
            err = dpl_sem_release(&inst->tx_sem);
            assert(err == DPL_OK);
        }

#if MYNEWT_VAL(DW1000_SYS_STATUS_BACKTRACE_LEN)
        if(!inst->sys_status_bt_lock && !inst->uwb_dev.status.autoack_triggered) {
            /* Assuming the start_tx writes the fctrl at send time */
            DW1000_SYS_STATUS_BT_FCTRL(inst, inst->uwb_dev.fctrl);
        }
#endif

        // Call the corresponding callback if present
        if(!(SLIST_EMPTY(&inst->uwb_dev.interface_cbs))){
            SLIST_FOREACH(cbs, &inst->uwb_dev.interface_cbs, next){
            if (cbs!=NULL && cbs->tx_complete_cb)
                if(cbs->tx_complete_cb((struct uwb_dev*)inst,cbs)) break;
            }
        }
    }
    // Tx buffer error
    if(inst->uwb_dev.status.txbuf_error){
        MAC_STATS_INC(TXBUF_err);
        dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_TXBERR, sizeof(uint32_t));
        if(dpl_sem_get_count(&inst->tx_sem) == 0){
            err = dpl_sem_release(&inst->tx_sem);
            assert(err == DPL_OK);
        }
    }

    // leading edge detection complete
    if(inst->sys_status & SYS_STATUS_LDEERR){
        MAC_STATS_INC(LDE_err);
        dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_LDEERR, sizeof(uint32_t));
    }

    // Handle frame reception/preamble detect timeout events
    if(inst->uwb_dev.status.rx_timeout_error){
        MAC_STATS_INC(RTO_cnt);
        dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_ALL_RX_TO, sizeof(uint32_t)); // Clear RX timeout event bits

        if (inst->control.abs_timeout) {
            /* Absolute timeout active, reactivate receiver if there's still time left */
            uint64_t systime = dw1000_read_systime(inst);
            uint32_t new_timeout = calc_rx_window_timeout(systime, inst->uwb_dev.abs_timeout);
            if (new_timeout > 1) {
                dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET+1, SYS_CTRL_RXENAB>>8, sizeof(uint8_t));
                dw1000_adj_rx_timeout(inst, new_timeout);
            } else {
                inst->control.abs_timeout = false;
            }
        }

        if (!inst->control.abs_timeout) {
            // Because of an issue with receiver restart after error conditions, an RX reset must be applied
            // after any error or timeout event to ensure the next good frame's timestamp is computed correctly.
            // See section "RX Message timestamp" in DW1000 User Manual.
            dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET, (uint16_t)SYS_CTRL_TRXOFF, sizeof(uint16_t)) ; // Disable the radio
            dw1000_phy_rx_reset(inst);

            inst->control.cir_enable = false;
            inst->control.rxauto_disable = false;
            inst->control.abs_timeout = false;

            // Call the corresponding frame services callback if present
            if(!(SLIST_EMPTY(&inst->uwb_dev.interface_cbs))){
                SLIST_FOREACH(cbs, &inst->uwb_dev.interface_cbs, next){
                    if (cbs!=NULL && cbs->rx_timeout_cb)
                        if(cbs->rx_timeout_cb((struct uwb_dev*)inst,cbs)) continue;
                }
            }
        }
    }

    // Handle RX errors events
    if(inst->uwb_dev.status.rx_error) {
        MAC_STATS_INC(RX_err);

        // Because of an issue with receiver restart after error conditions, an RX reset must be applied after any error or timeout event to ensure
        // the next good frame's timestamp is computed correctly.
        // See section "RX Message timestamp" in DW1000 User Manual.

        dw1000_write_reg(inst, SYS_STATUS_ID, 0, (SYS_STATUS_ALL_RX_ERR), sizeof(uint32_t)); // Clear RX error event bits

        if (inst->uwb_dev.config.dblbuffon_enabled && inst->uwb_dev.status.overrun_error) {
            MAC_STATS_INC(ROV_err);
            dw1000_phy_rx_reset(inst);
            dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET, 0b1, sizeof(uint8_t));
            dw1000_sync_rxbufptrs(inst);
        } else {
            dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET, (uint8_t) SYS_CTRL_TRXOFF, sizeof(uint8_t));
            dw1000_phy_rx_reset(inst);
        }
        /* Restart the receiver even if rxauto is not enabled. Timeout remain active if set.
         * NOTE: Because we reset the receiver explicitly above we will need to reenable
         * the receiver even though the auto-enable is on. */
        dw1000_write_reg(inst, SYS_CTRL_ID, SYS_CTRL_OFFSET+1, SYS_CTRL_RXENAB>>8, sizeof(uint8_t));
        if (inst->control.abs_timeout) {
            update_rx_window_timeout(inst, dw1000_read_systime(inst));
        }

        // Call the corresponding frame services callback if present
        if(!(SLIST_EMPTY(&inst->uwb_dev.interface_cbs))){
            SLIST_FOREACH(cbs, &inst->uwb_dev.interface_cbs, next){
            if (cbs!=NULL && cbs->rx_error_cb)
                if(cbs->rx_error_cb((struct uwb_dev*)inst,cbs)) continue;
            }
        }
    }

    /* Clear SLP2INIT event bits */
    if(inst->sys_status & SYS_STATUS_SLP2INIT){
        dw1000_write_reg(inst, SYS_STATUS_ID, 2, SYS_STATUS_SLP2INIT>>16, 1);
    }

    // Handle sleep timer event
    if(inst->sys_status & SYS_STATUS_CLKPLL_LL){
        dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_STATUS_CLKPLL_LL, sizeof(uint32_t));
        MAC_STATS_INC(PLL_LL_err);
    }
    // Handle sleep timer event
    if(inst->sys_status & SYS_MASK_MCPLOCK){
        dw1000_write_reg(inst, SYS_STATUS_ID, 0, SYS_MASK_MCPLOCK, sizeof(uint32_t));

        // restore antenna delay value, these are not preserved during sleep/deepsleep */
        dw1000_phy_set_rx_antennadelay(inst, inst->uwb_dev.rx_antenna_delay);
        dw1000_phy_set_tx_antennadelay(inst, inst->uwb_dev.tx_antenna_delay);

        // Call the corresponding callback if present
        inst->uwb_dev.status.sleeping = 0;
        if(!(SLIST_EMPTY(&inst->uwb_dev.interface_cbs))){
            SLIST_FOREACH(cbs, &inst->uwb_dev.interface_cbs, next){
            if (cbs!=NULL && cbs->sleep_cb)
                if (cbs->sleep_cb((struct uwb_dev*)inst,cbs)) continue;
            }
        }
    }

early_exit:
    dpl_sem_release(&inst->uwb_dev.irq_sem);
sem_error_exit:
    /* Check for possibly missed interrupts occuring whilst we were looking at this one
     * NOTE: Because the interrupt is edge based we will only register an event if the irq pin
     * goes low and then comes back up. If the pin is high now and no event is queued just after
     * swapping rx-buffers this means we didn't have time to finish reading the data
     * from the previous irq until a new one arrived -> queue another irq event for the task */
    if (hal_gpio_read(inst->irq_pin) && !dpl_event_is_queued(ev)) {
        dpl_eventq_put(&inst->uwb_dev.eventq, &inst->uwb_dev.interrupt_ev);
#if MYNEWT_VAL(DW1000_SYS_STATUS_BACKTRACE_LEN)
        if(!inst->sys_status_bt_lock) {
            DW1000_SYS_STATUS_BT_PTR(inst).interrupt_reentry = 1;
        }
#endif
    }

#if MYNEWT_VAL(DW1000_SYS_STATUS_BACKTRACE_LEN)
    if(!inst->sys_status_bt_lock) {
        DW1000_SYS_STATUS_BT_PTR(inst).utime_end = dpl_cputime_get32();
    }
#endif
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    dw1000_t *dev = (dw1000_t *) netdev;

    if (netdev == NULL) {
        return -ENODEV;
    }

    /* getting these options doesn't require the transceiver to be responsive */
    switch (opt) {
        default:
            /* Can still be handled in second switch */
            break;
    }

    int res;

    if (((res = netdev_ieee802154_get((netdev_ieee802154_t *)dev, opt, val,
                                      max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }

    /* temporarily wake up if sleeping */

    /* these options require the transceiver to be not sleeping*/
    switch (opt) {
        default:
            res = -ENOTSUP;
            break;
    }

    /* go back to sleep if were sleeping */

    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    dw1000_t *dev = (dw1000_t *) netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* temporarily wake up if sleeping and opt != NETOPT_STATE.
     * opt != NETOPT_STATE check prevents redundant wake-up. */

    switch (opt) {
        default:
            break;
    }

    /* go back to sleep if were sleeping and state hasn't been changed */

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)dev, opt, val, len);
    }

    return 0;
}
