#include "main.h"
#include "led_display.h"

/* 7SEG + EN (PNP) are active-low, traffic LEDs are active-high */
#define SEG_ON     GPIO_PIN_RESET
#define SEG_OFF    GPIO_PIN_SET
#define EN_ON      GPIO_PIN_RESET
#define EN_OFF     GPIO_PIN_SET
#define LIGHT_ON   GPIO_PIN_SET
#define LIGHT_OFF  GPIO_PIN_RESET

int led_buffer[MAX_LED] = { 0, 0, 0, 0 };

static GPIO_TypeDef* const segPort[7] =
    { SEG0_GPIO_Port, SEG1_GPIO_Port, SEG2_GPIO_Port, SEG3_GPIO_Port,
      SEG4_GPIO_Port, SEG5_GPIO_Port, SEG6_GPIO_Port };
static const uint16_t segPin[7] =
    { SEG0_Pin, SEG1_Pin, SEG2_Pin, SEG3_Pin, SEG4_Pin, SEG5_Pin, SEG6_Pin };

/* bit0 = a ... bit6 = g, 1 = segment lit (same digits as Lab 1 Ex4) */
static const uint8_t segCode[10] =
    { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };

static GPIO_TypeDef* const enPort[MAX_LED] =
    { EN0_GPIO_Port, EN1_GPIO_Port, EN2_GPIO_Port, EN3_GPIO_Port };
static const uint16_t enPin[MAX_LED] =
    { EN0_Pin, EN1_Pin, EN2_Pin, EN3_Pin };

void display7SEG(int num)
{
    if (num < 0 || num > 9) return;
    for (int i = 0; i < 7; i++) {
        HAL_GPIO_WritePin(segPort[i], segPin[i],
                          ((segCode[num] >> i) & 0x01) ? SEG_ON : SEG_OFF);
    }
}

void update7SEG(int index)
{
    for (int i = 0; i < MAX_LED; i++) {                 /* turn all digits off */
        HAL_GPIO_WritePin(enPort[i], enPin[i], EN_OFF);
    }
    if (index < 0 || index >= MAX_LED) return;
    display7SEG(led_buffer[index]);
    HAL_GPIO_WritePin(enPort[index], enPin[index], EN_ON);
}

/* Set by setDisplayValues() whenever the numbers change. */
static int display_changed = 0;

/* Call every SCAN_PERIOD. Shows one digit and returns the next index.
 * When the numbers change, the scan restarts from digit 0: the new value
 * appears right away (no lag behind the traffic lights) and all 4 digits of
 * a frame show the same value. With SCAN_PERIOD = 250 ms one frame lasts
 * exactly 1 s, so every countdown second shows exactly one full frame. */
int display_changed_flag(void)
{
    return display_changed;
}

int scan7SEG(int index)
{
    if (display_changed) {
        display_changed = 0;
        index = 0;
    }
    update7SEG(index);
    return (index + 1) % MAX_LED;
}

void setDisplayValues(int road1, int road2)
{
    led_buffer[0] = road1 / 10;
    led_buffer[1] = road1 % 10;
    led_buffer[2] = road2 / 10;
    led_buffer[3] = road2 % 10;
    display_changed = 1;
}

void setRoad1(enum Color c)
{
    HAL_GPIO_WritePin(RED1_GPIO_Port, RED1_Pin, (c == COLOR_RED)   ? LIGHT_ON : LIGHT_OFF);
    HAL_GPIO_WritePin(AMB1_GPIO_Port, AMB1_Pin, (c == COLOR_AMBER) ? LIGHT_ON : LIGHT_OFF);
    HAL_GPIO_WritePin(GRN1_GPIO_Port, GRN1_Pin, (c == COLOR_GREEN) ? LIGHT_ON : LIGHT_OFF);
}

void setRoad2(enum Color c)
{
    HAL_GPIO_WritePin(RED2_GPIO_Port, RED2_Pin, (c == COLOR_RED)   ? LIGHT_ON : LIGHT_OFF);
    HAL_GPIO_WritePin(AMB2_GPIO_Port, AMB2_Pin, (c == COLOR_AMBER) ? LIGHT_ON : LIGHT_OFF);
    HAL_GPIO_WritePin(GRN2_GPIO_Port, GRN2_Pin, (c == COLOR_GREEN) ? LIGHT_ON : LIGHT_OFF);
}
