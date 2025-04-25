#ifndef FOOT_CONTROLLER_H
#define FOOT_CONTROLLER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Configurar tempos em microssegundos
#define DEBOUNCE_TIME_US 200000      // Tempo de debouncing (50 ms)
#define LONG_PRESS_TIME_US 2000000 // Tempo para detectar long press (2 segundos)

// Definir os pinos dos botões
#define BUTTON1_PIN 16
#define BUTTON2_PIN 17
#define BUTTON3_PIN 18
#define BUTTON4_PIN 19

// Estrutura para armazenar o estado dos botões
typedef struct {
    uint8_t pin;
    bool pressed;
    bool long_press_detected;
    absolute_time_t press_time;
    absolute_time_t last_release_time;
} ButtonState;

// Ponteiros para funções de callback (podem ser sobrescritas no main.c)
extern void (*on_button_pressed)(uint8_t pin);
extern void (*on_button_released)(uint8_t pin);
extern void (*on_button_short_press)(uint8_t pin);
extern void (*on_button_long_press)(uint8_t pin);

// Função de interrupção para GPIO
void gpio_callback(uint gpio, uint32_t events);

// Função para inicializar os pinos dos botões
void init_buttons(void);

#endif // FOOT_CONTROLLER_H