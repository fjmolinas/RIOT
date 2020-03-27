/**
\brief This is a program to verify the correct functionality of the "debugpins"
       bsp module.

\note: Since the bsp modules for different platforms have the same declaration,
       you can use this project with any platform

Load this program on your board. When you run it, the error LED should blink.
Use a logic analyzer to see the activity on the 6 debug pins; one after another
they will transition 4 times.

\author Chuang Qian <cqian@berkeley.edu>, May 2012.
*/

#include "stdint.h"
#include "stdio.h"
// bsp modules required
#include "xtimer.h"
#include "board_ow.h"
#include "debugpins.h"
#include "debugpins_riot.h"

#define DELAY              (250 * 1000)

/**
\brief The program starts executing here.
*/
int main(void) {

   debugpins_riot_init(openwsn_debugpins_params);

   puts("frame pins");
   debugpins_frame_set();    xtimer_usleep(DELAY);
   debugpins_frame_toggle(); xtimer_usleep(DELAY);
   debugpins_frame_toggle(); xtimer_usleep(DELAY);
   debugpins_frame_clr();    xtimer_usleep(DELAY);

   puts("slot pins");
   debugpins_slot_set();     xtimer_usleep(DELAY);
   debugpins_slot_toggle();  xtimer_usleep(DELAY);
   debugpins_slot_toggle();  xtimer_usleep(DELAY);
   debugpins_slot_clr();     xtimer_usleep(DELAY);

   puts("fsm pins");
   debugpins_fsm_set();      xtimer_usleep(DELAY);
   debugpins_fsm_toggle();   xtimer_usleep(DELAY);
   debugpins_fsm_toggle();   xtimer_usleep(DELAY);
   debugpins_fsm_clr();      xtimer_usleep(DELAY);

   puts("task pins");
   debugpins_task_set();     xtimer_usleep(DELAY);
   debugpins_task_toggle();  xtimer_usleep(DELAY);
   debugpins_task_toggle();  xtimer_usleep(DELAY);
   debugpins_task_clr();     xtimer_usleep(DELAY);

   puts("isr pins");
   debugpins_isr_set();      xtimer_usleep(DELAY);
   debugpins_isr_toggle();   xtimer_usleep(DELAY);
   debugpins_isr_toggle();   xtimer_usleep(DELAY);
   debugpins_isr_clr();      xtimer_usleep(DELAY);

   puts("radio pins");
   debugpins_radio_set();    xtimer_usleep(DELAY);
   debugpins_radio_toggle(); xtimer_usleep(DELAY);
   debugpins_radio_toggle(); xtimer_usleep(DELAY);
   debugpins_radio_clr();    xtimer_usleep(DELAY);

   board_reset();

   return 0;
}