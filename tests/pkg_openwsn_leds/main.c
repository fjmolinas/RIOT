/**
\brief This is a program which shows how to use the bsp modules for the board
       and leds.

\note: Since the bsp modules for different platforms have the same declaration,
       you can use this project with any platform.

Load this program on your boards. The LEDs should start blinking furiously.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2014.
\author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, March 2019.
*/

#include "stdint.h"
#include "stdio.h"
// bsp modules required
#include "board_ow.h"
#include "xtimer.h"
#include "leds.h"
#include "ledpins_riot.h"

#define DELAY              (250 * 1000)

/**
\brief The program starts executing here.
*/
int main(void) {uint8_t i;

   ledpins_riot_init(openwsn_ledpins_params);

   while(1) {
      puts("Blink from beginning :-)");
      // error LED functions
      puts("leds_error_blink");
      leds_error_blink();       xtimer_usleep(DELAY);
      leds_error_off();         xtimer_usleep(DELAY);

      // radio LED functions
      puts("leds_radio_toggle");
      leds_radio_toggle();      xtimer_usleep(DELAY);
      leds_radio_off();         xtimer_usleep(DELAY);

      // sync LED functions
      puts("leds_sync_toggle");
      leds_sync_toggle();       xtimer_usleep(DELAY);
      leds_sync_off();          xtimer_usleep(DELAY);

      // debug LED functions
      puts("leds_debug_toggle");
      leds_debug_toggle();      xtimer_usleep(DELAY);
      leds_debug_off();         xtimer_usleep(DELAY);

      // all LED functions
      puts("leds_all_toggle");
      leds_all_off();           xtimer_usleep(DELAY);
      leds_all_on();            xtimer_usleep(DELAY);
      leds_all_off();           xtimer_usleep(DELAY);
      leds_all_toggle();        xtimer_usleep(DELAY);

      // LED increment function
      leds_all_off();           xtimer_usleep(DELAY);
      puts("leds_circular_shift");
      for (i=0;i<9;i++) {
         leds_circular_shift(); xtimer_usleep(DELAY);
      }
   }

   return 0;
}
