/*
 * Copyright (C) 2016 MUTEX NZ Ltd.
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Low-level radio driver for the CC2538
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 * @author      Ian Martin <ian@locicontrols.com>
 * @}
 */

#include "periph_conf.h"

#include "cc2538_rf.h"
#include "cc2538_rf_netdev.h"

#include "ztimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief MAC timer period
 *
 * The period is set to the CSMA-CA Backoff Period Unit (20 symbols, 320 us).
 * The system clock runs at 32 MHz. Thus, the timeout period is
 * 320us * 32MHz = ~10738 (0x29F2)
 */
#define TIMER_PERIOD_LSB           (0xF2)
#define TIMER_PERIOD_MSB           (0x29)

/**
 * @brief MAC timer period
 *
 * The period is set to the CSMA-CA Backoff Period Unit (20 symbols, 320 us).
 * The system clock runs at 32 MHz. Thus, the timeout period is
 * 320us * 32MHz = ~10738 (0x29F2)
 */
#define CCTEST_OBSSELX_EN           (BIT(7))

static void _cc2538_setup_mac_timer(void)
{
    RFCORE_SFR_MTMSEL &= ~CC2538_SFR_MTMSEL_MASK;
    /* Select timer period */
    RFCORE_SFR_MTMSEL |= CC2538_SFR_MTMSEL_TIMER_P;

    /* Fix timer to Backoff period */
    RFCORE_SFR_MTM0 |= TIMER_PERIOD_LSB;
    RFCORE_SFR_MTM1 |= TIMER_PERIOD_MSB;

    RFCORE_SFR_MTMSEL &= ~CC2538_SFR_MTMSEL_MASK;
    RFCORE_SFR_MTCTRL |= CC2538_MCTRL_SYNC_MASK | CC2538_MCTRL_RUN_MASK;
}

static void _cc2538_obsservable_signals(void)
{
    if (IS_ACTIVE(CONFIG_CC2538_RF_OBS_SIG)) {
        if (CONFIG_CC2538_RF_OBS_0 != disabled) {
            RFCORE_XREG_RFC_OBS_CTRL0 = CONFIG_CC2538_RF_OBS_0;
            *(&CCTEST_OBSSEL0 + CONFIG_CC2538_RF_OBS_SIG_0_PCX) = \
                CCTEST_OBSSELX_EN | rfc_obs_sig0;
        }
        if (CONFIG_CC2538_RF_OBS_1 != disabled) {
            RFCORE_XREG_RFC_OBS_CTRL1 = CONFIG_CC2538_RF_OBS_1;
            *(&CCTEST_OBSSEL0 + CONFIG_CC2538_RF_OBS_SIG_1_PCX) = \
                CCTEST_OBSSELX_EN | rfc_obs_sig1;
        }
        if (CONFIG_CC2538_RF_OBS_2 != disabled) {
            RFCORE_XREG_RFC_OBS_CTRL2 = CONFIG_CC2538_RF_OBS_2;
            *(&CCTEST_OBSSEL0 + CONFIG_CC2538_RF_OBS_SIG_2_PCX) = \
                CCTEST_OBSSELX_EN | rfc_obs_sig2;
        }
    }
}


bool cc2538_channel_clear(void)
{
    if (RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE == FSM_STATE_IDLE) {
        bool result;
        cc2538_on();
        RFCORE_WAIT_UNTIL(RFCORE->XREG_RSSISTATbits.RSSI_VALID);
        result = BOOLEAN(RFCORE->XREG_FSMSTAT1bits.CCA);
        cc2538_off();
        return result;
    }
    else {
        RFCORE_WAIT_UNTIL(RFCORE->XREG_RSSISTATbits.RSSI_VALID);
        return BOOLEAN(RFCORE->XREG_FSMSTAT1bits.CCA);
    }
}

void cc2538_init(void)
{
    /* Enable RF CORE clock in active mode */
    SYS_CTRL_RCGCRFC = 1UL;
    /* Enable  RF CORE  clock in sleep mode */
    SYS_CTRL_SCGCRFC = 1UL;
    /* Enable  RF CORE  clock in PM0 (system clock always powered down
        in PM1-3) */
    SYS_CTRL_DCGCRFC = 1UL;
    /* Wait for the clock enabling to take effect */
    while (!(SYS_CTRL_RCGCRFC & 1UL) || \
           !(SYS_CTRL_SCGCRFC & 1UL) || \
           !(SYS_CTRL_DCGCRFC & 1UL)
           ) {}

    /* Register Setting updates for optimal performance, RM section 23.15 */
    RFCORE_XREG_TXFILTCFG   = 0x09;
    RFCORE_XREG_AGCCTRL1    = 0x15;
    RFCORE_XREG_FSCAL1      = 0x01;
    ANA_REGS_IVCTRL         = 0x0B;

    /* Enable AUTOCRC and AUTOACK by default*/
    RFCORE_XREG_FRMCTRL0   = AUTOCRC | AUTOACK;

    /* Enable RX after TX? */
#if IS_ACTIVE(CONFIG_CC2538_RF_RX_AFTER_TX)
    RFCORE_XREG_FRMCTRL1   = SET_RXENMASK_ON_TX;
#else
    RFCORE_XREG_FRMCTRL1   = 0x00;
#endif

    /* Disable source address matching and pending bits */
    RFCORE_XREG_SRCMATCH = 0x00;

    /* Set FIFOP_THR to its max value*/
    RFCORE_XREG_FIFOPCTRL = CC2538_RF_MAX_DATA_LEN;

    /* Set default IRQ */
    RFCORE_XREG_RFIRQM0 = RXPKTDONE | SFD | FIFOP;
#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
    RFCORE_XREG_RFIRQM1 = TXDONE | CSP_STOP;
#else
    RFCORE_XREG_RFIRQM1 = TXDONE;
#endif

    /* Enable all RF CORE error interrupts */
    RFCORE_XREG_RFERRM = STROBE_ERR | TXUNDERF | TXOVERF | \
                         RXUNDERF | RXOVERF | NLOCK;

    _cc2538_obsservable_signals();

    /* Enable IRQs */
    if (SYS_CTRL->I_MAP) {
        NVIC_SetPriority(RF_RXTX_ALT_IRQn, CPU_DEFAULT_IRQ_PRIO);
        NVIC_EnableIRQ(RF_RXTX_ALT_IRQn);

        NVIC_SetPriority(RF_ERR_ALT_IRQn, CPU_DEFAULT_IRQ_PRIO);
        NVIC_EnableIRQ(RF_ERR_ALT_IRQn);

        NVIC_SetPriority(MAC_TIMER_ALT_IRQn, CPU_DEFAULT_IRQ_PRIO);
        NVIC_EnableIRQ(MAC_TIMER_ALT_IRQn);
    }
    else {
        NVIC_SetPriority(RF_RXTX_IRQn, CPU_DEFAULT_IRQ_PRIO);
        NVIC_EnableIRQ(RF_RXTX_IRQn);

        NVIC_SetPriority(RF_ERR_IRQn, CPU_DEFAULT_IRQ_PRIO);
        NVIC_EnableIRQ(RF_ERR_IRQn);

        NVIC_SetPriority(MACTIMER_IRQn, CPU_DEFAULT_IRQ_PRIO);
        NVIC_EnableIRQ(MACTIMER_IRQn);
    }

    /* setup mac timer */
    _cc2538_setup_mac_timer();

    /* Flush the receive and transmit FIFOs */
    RFCORE_SFR_RFST = ISFLUSHTX;
    RFCORE_SFR_RFST = ISFLUSHRX;
}

bool cc2538_is_on(void)
{
    return RFCORE->XREG_FSMSTAT1bits.RX_ACTIVE || \
           RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE;
}

void cc2538_off(void)
{
    /* Wait for ongoing TX to complete (e.g. this could be an outgoing ACK) */
    RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE == 0);

    /* Don't turn off if we are off as this will trigger a Strobe Error */
    if (RFCORE_XREG_RXENABLE != 0) {
        RFCORE_SFR_RFST = ISRFOFF;
        /* Clear FIFO IRQ */
        RFCORE_SFR_RFIRQF0 &= ~(RXPKTDONE | FIFOP | SFD);
    }
}

bool cc2538_on(void)
{
    /* Flush RX FIFO and reset demodulator */
    RFCORE_SFR_RFST = ISFLUSHRX;

    /* Enable/calibrate RX */
    RFCORE_SFR_RFST = ISRXON;

    return true;
}

void cc2538_setup(cc2538_rf_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &cc2538_rf_driver;

    cc2538_init();

    netdev_register(netdev, NETDEV_CC2538, 0);

    cc2538_set_tx_power(CC2538_RF_POWER_DEFAULT);
    cc2538_set_chan(CC2538_RF_CHANNEL_DEFAULT);

    /* assign default addresses */
    netdev_ieee802154_setup(&dev->netdev);
    cc2538_set_addr_long(dev->netdev.long_addr);
    cc2538_set_addr_short(dev->netdev.short_addr);

    cc2538_on();
}

void cc2538_tx_exec(const cc2538_rf_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    RFCORE_SFR_RFST = ISTXON;

    /* Transmission does not trigger an SFD interrupt, so we need to
       poll and wait fot the SFD bit to be set */
    while (RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE) {
        if(RFCORE->XREG_FSMSTAT1bits.SFD) {
            DEBUG_PUTS("[cc2538_rf] EVT - TX_STARTED");
            netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
            break;
        } else {
            thread_yield();
        }
    }

    /* Wait for transmission to complete */
    RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE == 0);
}
