#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

/* TIM2 interrupt period in ms.
 * Ex4: if the TIM2 period is changed (e.g. 1 ms or 100 ms),
 * change ONLY this value - every duration below is computed from it. */
#define TIMER_CYCLE     10

/* Software timer IDs */
#define TIMER_SECOND    0   /* 1 s tick for the traffic-light countdown */
#define TIMER_SCAN      1   /* 7-segment scanning                       */
#define TIMER_BLINK     2   /* 2 Hz blinking in modes 2-4               */
#define TIMER_HEARTBEAT 3   /* LED_RED on PA5: shows the MCU is alive   */
#define NUM_TIMERS      4

extern volatile int timer_flag[NUM_TIMERS];

void setTimer(int id, int duration_ms);
void timer_run(void);   /* call in HAL_TIM_PeriodElapsedCallback */

#endif /* INC_SOFTWARE_TIMER_H_ */
