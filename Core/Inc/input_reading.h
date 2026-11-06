#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#define BTN_MODE  0   /* BUTTON_1 - PA0: change mode      */
#define BTN_INC   1   /* BUTTON_2 - PA1: increase value   */
#define BTN_SET   2   /* BUTTON_3 - PA2: save value       */

void button_reading(void);          /* call in HAL_TIM_PeriodElapsedCallback */
int  is_button_pressed(int index);  /* returns 1 ONCE per press, then clears  */

#endif /* INC_INPUT_READING_H_ */
