#include "stdint.h"
#include "string.h"

#include "board.h"
#include "debugpins.h"
#include "sctimer.h"

#include "ps.h"

//=========================== defines =========================================

#define SCTIMER_PERIOD     (32768) // @32kHz = 1s

//=========================== prototypes ======================================

void cb_compare(void);

//=========================== main ============================================

/**
\brief The program starts executing here.
*/
int main(void)
{
   sctimer_init();
   sctimer_set_callback(cb_compare);
   /* bootstrapping repeating callback by calling it manually once */
   cb_compare();
}

//=========================== callbacks =======================================
#define REPETITIONS 100
uint32_t cnt=0;
uint32_t durations[] = { 1, 2, 5, 10, 100, 200};
uint32_t duration_conf = 0;
void cb_compare(void) {
   uint32_t now = sctimer_readCounter();
   cnt++;
   if (cnt == REPETITIONS) {
       printf("%"PRIu32" ms  %"PRIu32" times: %"PRIu32"\n", durations[duration_conf], cnt, now);
       duration_conf++;
       duration_conf %= (sizeof(durations)/sizeof(durations[0]));
       cnt = 0;
   }
   //uint32_t next_wakeup = now + SCTIMER_PERIOD;
   //uint32_t next_wakeup = now + (2 * 32768) / 1000;
   uint32_t next_wakeup = now + (durations[duration_conf] * 32768) / 1000;
   sctimer_setCompare(next_wakeup);

   LED0_TOGGLE;
   //sctimer_setCompare(next_wakeup);
}
