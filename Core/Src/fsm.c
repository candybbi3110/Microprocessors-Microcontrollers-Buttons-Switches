#include "main.h"
#include "fsm.h"
#include "software_timer.h"
#include "input_reading.h"
#include "led_display.h"

#define MAX_VALUE          99
#define BLINK_HALF_PERIOD  250   /* toggle every 250 ms -> 2 Hz */

/* Default durations in seconds (from Lab 1): red = amber + green */
static int durRed = 5, durAmber = 2, durGreen = 3;

enum Mode  { MODE_NORMAL = 1, MODE_RED = 2, MODE_AMBER = 3, MODE_GREEN = 4 };
enum Phase { RED1_GREEN2, RED1_AMBER2, GREEN1_RED2, AMBER1_RED2 };

static enum Mode  mode  = MODE_NORMAL;
static enum Phase phase = RED1_GREEN2;
static int cnt1, cnt2;     /* seconds left on road 1 / road 2 */
static int temp;           /* value being edited in modes 2-4 */
static int blinkOn;

/* ---------------- Traffic FSM (mode 1) ---------------- */
static void enterPhase(enum Phase p)
{
    phase = p;
    switch (p) {
    case RED1_GREEN2:
        cnt1 = durRed;   cnt2 = durGreen;
        setRoad1(COLOR_RED);   setRoad2(COLOR_GREEN);
        break;
    case RED1_AMBER2:                     /* road 1 keeps counting its red */
        cnt2 = durAmber;
        setRoad1(COLOR_RED);   setRoad2(COLOR_AMBER);
        break;
    case GREEN1_RED2:
        cnt1 = durGreen; cnt2 = durRed;
        setRoad1(COLOR_GREEN); setRoad2(COLOR_RED);
        break;
    case AMBER1_RED2:                     /* road 2 keeps counting its red */
        cnt1 = durAmber;
        setRoad1(COLOR_AMBER); setRoad2(COLOR_RED);
        break;
    }
    setDisplayValues(cnt1, cnt2);
}

static void trafficTick(void)             /* called once per second */
{
    cnt1--;
    cnt2--;
    switch (phase) {
    case RED1_GREEN2: if (cnt2 <= 0) enterPhase(RED1_AMBER2); break;
    case RED1_AMBER2: if (cnt2 <= 0) enterPhase(GREEN1_RED2); break;
    case GREEN1_RED2: if (cnt1 <= 0) enterPhase(AMBER1_RED2); break;
    case AMBER1_RED2: if (cnt1 <= 0) enterPhase(RED1_GREEN2); break;
    }
    setDisplayValues(cnt1, cnt2);
}

/* ---------------- Helpers for modes 2-4 ---------------- */
static int currentValue(enum Mode m)
{
    if (m == MODE_RED)   return durRed;
    if (m == MODE_AMBER) return durAmber;
    return durGreen;
}

static enum Color editColor(enum Mode m)
{
    if (m == MODE_RED)   return COLOR_RED;
    if (m == MODE_AMBER) return COLOR_AMBER;
    return COLOR_GREEN;
}

/* Keep red = amber + green; invalid values are not saved */
static void saveValue(void)
{
    switch (mode) {
    case MODE_RED:
        if (temp > durAmber) { durRed = temp; durGreen = temp - durAmber; }
        break;
    case MODE_AMBER:
        if (temp + durGreen <= MAX_VALUE) { durAmber = temp; durRed = temp + durGreen; }
        break;
    case MODE_GREEN:
        if (durAmber + temp <= MAX_VALUE) { durGreen = temp; durRed = durAmber + temp; }
        break;
    default:
        break;
    }
}

/* ---------------- Mode FSM ---------------- */
static void enterMode(enum Mode m)
{
    mode = m;
    if (m == MODE_NORMAL) {
        enterPhase(RED1_GREEN2);              /* restart with new durations */
        setTimer(TIMER_SECOND, 1000);
    } else {
        temp = currentValue(m);               /* unsaved edits are discarded */
        blinkOn = 0;
        setRoad1(COLOR_OFF);
        setRoad2(COLOR_OFF);
        setTimer(TIMER_BLINK, BLINK_HALF_PERIOD);
        setDisplayValues(temp, m);            /* road 1: value, road 2: mode */
    }
}

void fsm_init(void)
{
    enterMode(MODE_NORMAL);
}

void fsm_run(void)
{
    if (is_button_pressed(BTN_MODE)) {
        enterMode((mode == MODE_GREEN) ? MODE_NORMAL : (enum Mode)(mode + 1));
    }

    switch (mode) {
    case MODE_NORMAL:
        is_button_pressed(BTN_INC);           /* ignore INC/SET in mode 1 */
        is_button_pressed(BTN_SET);
        if (timer_flag[TIMER_SECOND]) {
            setTimer(TIMER_SECOND, 1000);
            trafficTick();
        }
        break;

    default:                                   /* modes 2, 3, 4 */
        if (is_button_pressed(BTN_INC)) {
            temp++;
            if (temp > MAX_VALUE) temp = 1;
            setDisplayValues(temp, mode);
        }
        if (is_button_pressed(BTN_SET)) {
            saveValue();
            temp = currentValue(mode);         /* shows the saved value */
            setDisplayValues(temp, mode);
        }
        if (timer_flag[TIMER_BLINK]) {
            setTimer(TIMER_BLINK, BLINK_HALF_PERIOD);
            blinkOn = !blinkOn;
            enum Color c = blinkOn ? editColor(mode) : COLOR_OFF;
            setRoad1(c);
            setRoad2(c);
        }
        break;
    }
}
