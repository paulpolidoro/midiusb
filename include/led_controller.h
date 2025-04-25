#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdio.h>
#include <stdint.h>
#include "hardware/gpio.h"
#include "pico/time.h"

// Define os pinos dos LEDs
#define LEDPOWER_PIN 15
#define LEDTAP_PIN 14
#define LED1_PIN 13
#define LED2_PIN 12
#define LED3_PIN 11
#define LED4_PIN 10

// Inicializa os LEDs
void init_leds(void);

// Liga o LED correspondente ao pino
void led_on(uint pin);

// Desliga o LED correspondente ao pino
void led_off(uint pin);

// Pisca o LED de forma contínua e independente
void led_blink(uint pin, uint interval_ms);

#endif // LED_CONTROLLER_H