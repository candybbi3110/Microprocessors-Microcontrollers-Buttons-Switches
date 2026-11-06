#include "main.h"
#include "input_reading.h"
#include "software_timer.h"

#define NO_OF_BUTTONS       3
#define BUTTON_IS_PRESSED   GPIO_PIN_RESET   /* pull-up: pressed = 0 */
#define BUTTON_IS_RELEASED  GPIO_PIN_SET

/* Sample the buttons every ~10 ms whatever TIMER_CYCLE is (Ex4). */
#define SAMPLE_TICKS        ((10 / TIMER_CYCLE) > 0 ? (10 / TIMER_CYCLE) : 1)
#define SAMPLE_PERIOD_MS    (SAMPLE_TICKS * TIMER_CYCLE)
#define LONG_PRESS_SAMPLES  (1000 / SAMPLE_PERIOD_MS)   /* hold > 1 s      */
#define AUTO_REPEAT_SAMPLES (500  / SAMPLE_PERIOD_MS)   /* then every 0.5 s */

static GPIO_TypeDef* const btnPort[NO_OF_BUTTONS] =
    { BUTTON_1_GPIO_Port, BUTTON_2_GPIO_Port, BUTTON_3_GPIO_Port };
static const uint16_t btnPin[NO_OF_BUTTONS] =
    { BUTTON_1_Pin, BUTTON_2_Pin, BUTTON_3_Pin };

/* Only the INC button auto-repeats when held */
static const uint8_t autoRepeat[NO_OF_BUTTONS] = { 0, 1, 0 };

/* Three consecutive samples must agree (debouncing, N = 3) */
static GPIO_PinState reg0[NO_OF_BUTTONS]   = { GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET };
static GPIO_PinState reg1[NO_OF_BUTTONS]   = { GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET };
static GPIO_PinState reg2[NO_OF_BUTTONS]   = { GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET };
static GPIO_PinState stable[NO_OF_BUTTONS] = { GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET };

static int holdCounter[NO_OF_BUTTONS];
static volatile uint8_t pressFlag[NO_OF_BUTTONS];
static int sampleCounter = 0;

void button_reading(void)
{
    static int initialized = 0;
    if (!initialized) {             /* take the REAL pin level at start-up so a  */
        for (int i = 0; i < NO_OF_BUTTONS; i++) {   /* slow-rising pull-up is   */
            GPIO_PinState s = HAL_GPIO_ReadPin(btnPort[i], btnPin[i]); /* not   */
            reg0[i] = reg1[i] = reg2[i] = stable[i] = s;  /* seen as a press    */
        }
        initialized = 1;
        return;
    }

    if (++sampleCounter < SAMPLE_TICKS) return;
    sampleCounter = 0;

    for (int i = 0; i < NO_OF_BUTTONS; i++) {
        reg2[i] = reg1[i];
        reg1[i] = reg0[i];
        reg0[i] = HAL_GPIO_ReadPin(btnPort[i], btnPin[i]);

        if (reg0[i] == reg1[i] && reg1[i] == reg2[i]) {      /* stable reading */
            if (stable[i] != reg0[i]) {                       /* state changed  */
                stable[i] = reg0[i];
                if (stable[i] == BUTTON_IS_PRESSED) {         /* falling edge   */
                    pressFlag[i] = 1;
                    holdCounter[i] = LONG_PRESS_SAMPLES;
                }
            } else if (stable[i] == BUTTON_IS_PRESSED && autoRepeat[i]) {
                if (--holdCounter[i] <= 0) {                  /* still held     */
                    pressFlag[i] = 1;
                    holdCounter[i] = AUTO_REPEAT_SAMPLES;
                }
            }
        }
    }
}

int is_button_pressed(int index)
{
    if (index < 0 || index >= NO_OF_BUTTONS) return 0;
    if (pressFlag[index]) {
        pressFlag[index] = 0;
        return 1;
    }
    return 0;
}
