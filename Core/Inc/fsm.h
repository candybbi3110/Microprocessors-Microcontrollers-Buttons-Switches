#ifndef INC_FSM_H_
#define INC_FSM_H_

void fsm_init(void);   /* call once after HAL_TIM_Base_Start_IT */
void fsm_run(void);    /* call in while(1)                      */

#endif /* INC_FSM_H_ */
