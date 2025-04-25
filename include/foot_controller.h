#ifndef FOOT_CONTROLLER_H
#define FOOT_CONTROLLER_H

#include "pico/stdlib.h"


#define DEBOUNCE_TIME_US 200000      // Tempo de debouncing (50 ms)
#define LONG_PRESS_TIME_US 2000000 // Tempo para detectar long press (2 segundos)
#define TOTAL_FOOTS 4

#define FOOT1_PIN 16
#define FOOT2_PIN 17
#define FOOT3_PIN 18
#define FOOT4_PIN 19

typedef struct {
    int pin;
    bool pressed;
    bool long_press_detected;
    absolute_time_t press_time;
    absolute_time_t last_release_time;
} FootState;

extern void (*on_foot_pressed)(uint8_t pin);
extern void (*on_foot_released)(uint8_t pin);
extern void (*on_foot_short_press)(uint8_t pin);
extern void (*on_foot_long_press)(uint8_t pin);


void init_foots(void);

#endif