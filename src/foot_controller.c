#include "foot_controller.h"
#include "hardware/gpio.h"
#include <stdio.h>

FootState foots[TOTAL_FOOTS] = {
    {FOOT1_PIN, false, 0, 0},
    {FOOT2_PIN, false, 0, 0},
    {FOOT3_PIN, false, 0, 0},
    {FOOT4_PIN, false, 0, 0}
};

// Inicializa os ponteiros com as funções padrã
void (*on_foot_pressed)(uint8_t pin);
void (*on_foot_released)(uint8_t pin);
void (*on_foot_short_press)(uint8_t pin);
void (*on_foot_long_press)(uint8_t pin);


bool check_long_press(struct repeating_timer *t) {
    for (int i = 0; i < TOTAL_FOOTS; i++) {
        if (foots[i].pressed) {
            const absolute_time_t now = get_absolute_time();
            const int64_t press_duration = absolute_time_diff_us(foots[i].press_time, now);

            if (press_duration >= LONG_PRESS_TIME_US && !foots[i].long_press_detected) {
                on_foot_long_press(foots[i].pin);
                foots[i].long_press_detected = true;
            }
        }
    }
    return true;
}

void gpio_callback(const int pin, int events) {
    for (int i = 0; i < TOTAL_FOOTS; i++) {
        if (foots[i].pin == pin) {
            const absolute_time_t now = get_absolute_time();

            // Verifica diretamente o estado atual do GPIO
            if (gpio_get(pin) == 0) { // Botão está pressionado (LOW)
                const int64_t time_since_release = absolute_time_diff_us(foots[i].last_release_time, now);

                if (time_since_release > DEBOUNCE_TIME_US) { // Ignora eventos dentro do delay
                    if (!foots[i].pressed) { // Apenas registra se ainda não foi detectado
                        foots[i].pressed = true;
                        foots[i].press_time = now;
                        foots[i].long_press_detected = false; // Redefine o estado de long press
                        on_foot_pressed(pin);
                    }
                }
            } else { // Botão está liberado (HIGH)
                if (foots[i].pressed) { // Apenas registra se estava pressionado
                    foots[i].pressed = false;

                    // Calcula o tempo de pressionamento
                    const int64_t press_duration = absolute_time_diff_us(foots[i].press_time, now);

                    // Verifica short press
                    if (!foots[i].long_press_detected && press_duration < LONG_PRESS_TIME_US) {
                        on_foot_short_press(pin);
                    }

                    on_foot_released(pin);

                    // Registra o momento do release
                    foots[i].last_release_time = now;
                }
            }
        }
    }
}

/**
 * Inicializa os
 */
void init_foots(void) {
    for (int i = 0; i < TOTAL_FOOTS; i++) {
        gpio_init(foots[i].pin);
        gpio_set_dir(foots[i].pin, GPIO_IN);
        gpio_pull_up(foots[i].pin);
        gpio_set_irq_enabled_with_callback(
            foots[i].pin,
            GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
            true,
            gpio_callback
        );
    }

    static repeating_timer_t timer;
    add_repeating_timer_ms(100, check_long_press, NULL, &timer);
}