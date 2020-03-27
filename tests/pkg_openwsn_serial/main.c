/**
\brief This is a program which shows how to use the bsp modules for the board
       and UART.

\note: Since the bsp modules for different platforms have the same declaration,
       you can use this project with any platform.

Load this program on your board. Open a serial terminal client (e.g. PuTTY or
TeraTerm):
- You will read "Hello World!" printed over and over on your terminal client.
- when you enter a character on the client, the board echoes it back (i.e. you
  see the character on the terminal client) and the "ERROR" led blinks.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
*/

#include <stdatomic.h>

#include "stdint.h"
#include "stdio.h"
// bsp modules required

#include "board_ow.h"
#include "uart_ow.h"
#include "sctimer.h"
#include "leds.h"
#include "ledpins_riot.h"

#include "debugpins.h"
#include "debugpins_riot.h"

// driver modules required
#include "openserial.h"
//=========================== defines =========================================

#define SCTIMER_PERIOD 328          // 328@32kHz ~ 10ms
static char stringToPrint[]       = "02drv_openserial\r\n";

//=========================== variables =======================================

typedef void (*task_cbt)(void);

typedef enum {
   TASKPRIO_NONE                  = 0x00,
   TASKPRIO_MAX                   = 0x01,
} task_prio_t;

typedef struct {
   atomic_bool        timerFired;
   atomic_bool        fInhibit;
} app_vars_t;

static app_vars_t app_vars;
static open_addr_t addr;

//=========================== prototypes ======================================

void cb_compare(void);


//=========================== main ============================================

/**
\brief The program starts executing here.
*/
int main(void)
{
   // clear local variable
   memset(&addr,0,sizeof(open_addr_t));
   memset(&app_vars,0,sizeof(app_vars_t));

   // initialize the board
   board_init_openwsn();
   openserial_init();

   // setup sctimer
   sctimer_set_callback(cb_compare);
   sctimer_setCompare(sctimer_readCounter()+SCTIMER_PERIOD);

   while(1) {
      // while(app_vars.timerFired == FALSE);
      if (app_vars.timerFired == TRUE) {
         app_vars.timerFired = FALSE;
         openserial_print_str(stringToPrint, sizeof(stringToPrint));
         if (app_vars.fInhibit==TRUE) {
            openserial_inhibitStart();
            app_vars.fInhibit = FALSE;
         } else {
            openserial_inhibitStop();
            app_vars.fInhibit = TRUE;
         }
      }
   }
}

//=========================== callbacks =======================================

void cb_compare(void) {
   leds_radio_toggle();
   app_vars.timerFired = TRUE;
   sctimer_setCompare(sctimer_readCounter()+SCTIMER_PERIOD);
}

//=========================== stub functions ==================================

open_addr_t* idmanager_getMyID(uint8_t type) {
   return &addr;
}

void scheduler_push_task(task_cbt task_cb, task_prio_t prio){}

void ieee154e_getAsn(uint8_t* array) {
   array[0]   = 0x00;
   array[1]   = 0x01;
   array[2]   = 0x02;
   array[3]   = 0x03;
   array[4]   = 0x04;
}

void idmanager_setJoinKey(uint8_t *key) {}
void idmanager_triggerAboutRoot(void) {}
void openbridge_triggerData(void) {}
void tcpinject_trigger(void) {}
void udpinject_trigger(void) {}
void icmpv6echo_trigger(void) {}
void icmpv6rpl_setDIOPeriod(uint16_t dioPeriod){};
void icmpv6rpl_setDAOPeriod(uint16_t daoPeriod){};
void icmpv6echo_setIsReplyEnabled(bool isEnabled){}
void sixtop_setEBPeriod(uint8_t ebPeriod){}
void sixtop_setKaPeriod(uint16_t kaPeriod){}
void sixtop_setHandler(void){}
owerror_t sixtop_request(
    uint8_t      code,
    open_addr_t* neighbor,
    uint8_t      numCells,
    uint8_t      cellOptions,
    cellInfo_ht* celllist_toBeAdded,
    cellInfo_ht* celllist_toBeDeleted,
    uint8_t      sfid,
    uint16_t     listingOffset,
    uint16_t     listingMaxNumCells
){return 0;}
void sixtop_addORremoveCellByInfo(void){}
void sixtop_setIsResponseEnabled(bool isEnabled){}
void icmpv6rpl_setMyDAGrank(dagrank_t rank){}
bool icmpv6rpl_getPreferredParentIndex(uint8_t* indexptr){return TRUE;}
bool icmpv6rpl_getPreferredParentEui64(open_addr_t* addressToWrite){return TRUE;}
void schedule_setFrameLength(uint16_t newFrameLength){}
void ieee154e_setSlotDuration(uint16_t duration){}
void ieee154e_setIsSecurityEnabled(bool isEnabled){}
void ieee154e_setIsAckEnabled(bool isEnabled){}
void ieee154e_setSingleChannel(uint8_t channel){}
void sniffer_setListeningChannel(uint8_t channel){}
void msf_appPktPeriod(uint8_t numAppPacketsPerSlotFrame){}
uint8_t msf_getsfid(void) {return 0;}

bool debugPrint_isSync(void) {
   return FALSE;
}
bool debugPrint_id(void) {
   return FALSE;
}
bool debugPrint_kaPeriod(void) {
   return FALSE;
}
bool debugPrint_myDAGrank(void) {
   return FALSE;
}
bool debugPrint_asn(void) {
   return FALSE;
}
bool debugPrint_macStats(void) {
   return FALSE;
}
bool debugPrint_schedule(void) {
   return FALSE;
}
bool debugPrint_backoff(void) {
   return FALSE;
}
bool debugPrint_queue(void) {
   return FALSE;
}
bool debugPrint_neighbors(void) {
   return FALSE;
}
bool debugPrint_joined(void) {
   return FALSE;
}
