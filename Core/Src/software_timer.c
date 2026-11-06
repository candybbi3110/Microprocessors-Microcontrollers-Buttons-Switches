#include "software_timer.h"

/* Counters are kept in MILLISECONDS, not in ticks.
 * Each interrupt subtracts TIMER_CYCLE. When a timer expires, the overshoot
 * (negative remainder) is carried into the next setTimer() call, so the
 * AVERAGE period stays exact even if the duration is not a multiple of
 * TIMER_CYCLE (e.g. 250 ms with a 100 ms tick -> 300, 200, 300, 200 ...
 * = 250 ms on average -> still 2 Hz). */
static volatile int timer_counter[NUM_TIMERS];
volatile int timer_flag[NUM_TIMERS];

void setTimer(int id, int duration_ms)
{
    if (id < 0 || id >= NUM_TIMERS) return;

    int carry = (timer_counter[id] < 0) ? timer_counter[id] : 0;
    int value = duration_ms + carry;
    if (value <= 0) value = TIMER_CYCLE;      /* at least one tick */

    timer_counter[id] = value;
    timer_flag[id] = 0;
}

void timer_run(void)
{
    for (int i = 0; i < NUM_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i] -= TIMER_CYCLE;
            if (timer_counter[i] <= 0) {
                timer_flag[i] = 1;
            }
        }
    }
}
