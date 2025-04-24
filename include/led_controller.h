#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "pico/stdlib.h"

// Definições dos pinos dos LEDs
#define LEDPOWER_PIN 2
#define LEDTAP_PIN 3
#define LED1_PIN 4
#define LED2_PIN 5
#define LED3_PIN 6
#define LED4_PIN 7

// Inicializa todos os LEDs
void init_leds(void);

// Liga o LED
void on(uint pin);

// Desliga o LED
void off(uint pin);

// Pisca o LED com um intervalo de tempo especificado
void blink(uint pin, uint interval_ms);

#endif // LED_CONTROLLER_H
