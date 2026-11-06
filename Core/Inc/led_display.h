#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#define MAX_LED 4

enum Color { COLOR_OFF = 0, COLOR_RED, COLOR_AMBER, COLOR_GREEN };

extern int led_buffer[MAX_LED];

/* 7-segment (reused from Lab 1 Ex4 and Lab 2 Ex3) */
void display7SEG(int num);
void update7SEG(int index);
int  scan7SEG(int index);                       /* returns next index */
int  display_changed_flag(void);               /* 1 if the numbers changed */
void setDisplayValues(int road1, int road2);   /* fills led_buffer */

/* Traffic lights: road 1 = N/S (RED1, AMB1, GRN1), road 2 = E/W */
void setRoad1(enum Color c);
void setRoad2(enum Color c);

#endif /* INC_LED_DISPLAY_H_ */
