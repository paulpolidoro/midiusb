#ifndef FOOT_CONTROLLER_H
#define FOOT_CONTROLLER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definir os pinos dos botões
#define BUTTON1_PIN 2
#define BUTTON2_PIN 3
#define BUTTON3_PIN 4
#define BUTTON4_PIN 5

// Estrutura para armazenar o estado dos botões
typedef struct {
    uint8_t pin;
    bool pressed;
    absolute_time_t press_time;
} ButtonState;

// Funções de callback
void on_button_pressed(uint8_t pin);
void on_button_released(uint8_t pin);
void on_button_short_press(uint8_t pin);
void on_button_long_press(uint8_t pin);
void on_buttons_combination(uint8_t pin1, uint8_t pin2);

// Função de interrupção GPIO
void gpio_callback(uint gpio, uint32_t events);

// Inicializar os botões
void init_buttons(void);

#endif // FOOT_CONTROLLER_H
