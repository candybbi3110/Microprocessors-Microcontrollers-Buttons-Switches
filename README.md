# Lab 3 – Buttons / Switches (CO3010, HCMUT)

Traffic light with buttons on the **STM32F103C6**, built with STM32CubeIDE and simulated in Proteus.

- **Student:** Phạm Bùi Phương Trân – 2453286 – CC03
- **Course:** Microprocessors – Microcontrollers (CO3010)

## Features
- Traffic light at a cross road: 12 LEDs, four 7-segment displays, three buttons
- 4 modes: mode 1 runs the traffic light; modes 2, 3, 4 change the duration of the red, amber and green LEDs (LEDs blink at 2 Hz)
- Button debouncing (3 consecutive samples every 10 ms), long press auto-repeat for the INC button
- Red = amber + green is always kept; invalid values are not saved
- Non-blocking: no `HAL_Delay()`, a 10 ms TIM2 interrupt only updates software timers and samples the buttons

## Files
| File | Content |
|---|---|
| `Core/Src/fsm.c`, `Core/Inc/fsm.h` | Mode FSM and traffic FSM |
| `Core/Src/input_reading.c`, `Core/Inc/input_reading.h` | Button reading and debouncing |
| `Core/Src/software_timer.c`, `Core/Inc/software_timer.h` | Software timers |
| `Core/Src/led_display.c`, `Core/Inc/led_display.h` | 7-segment scanning and traffic LEDs |
| `Core/Src/main.c` | Initialisation, main loop, timer callback |
| `Lab3.ioc` | STM32CubeMX configuration |
| `Lab3.pdsprj`, `Lab3.hex` | Proteus project and compiled program |

## Buttons
| Button | Function |
|---|---|
| BTN1 (MODE) | Go to the next mode (1 → 2 → 3 → 4 → 1) |
| BTN2 (INC) | Increase the value (1–99), hold to repeat every 0.5 s |
| BTN3 (SET) | Save the value |
