#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H


#include "pico/time.h"


#define LEDPOWER_PIN 15
#define LEDTAP_PIN 14
#define LED1_PIN 13
#define LED2_PIN 12
#define LED3_PIN 11
#define LED4_PIN 10

typedef struct {
    int pin;                   // Pino do LED
    bool is_blinking;          // Indica se o LED está piscando
    repeating_timer_t timer;   // Timer para controlar o blinking
    int interval_ms;           // Intervalo de piscar (em ms)
    bool led_state;            // Estado atual do LED (ligado/desligado)
    int timeout_ms;            // Tempo limite para o blinking (em ms)
    int elapsed_ms;
    bool before_blink;// Tempo já decorrido (em ms)
} LEDState;


void init_leds(void);

/**
 * Liga o LED
 * @param pin
 */
void led_on(int pin);

/**
 * Desliga o LED
 * @param pin
 */
void led_off(int pin);

/**
 * Liga ou desliga o LED de acordo com seu status atual
 * @param pin
 */
void led_toggle(int pin);

/**
 * PIsca o LED em um intervalo determinado
 * @param pin
 * @param interval_ms
 * @param timeout_ms
 */
void led_blink(int pin, int interval_ms, int timeout_ms);

#endif
