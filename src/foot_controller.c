#include "foot_controller.h"

// Inicializa os botões com seus pinos correspondentes
ButtonState buttons[4] = {
    {BUTTON1_PIN, false, 0, 0}, // Adicionado last_release_time como 0
    {BUTTON2_PIN, false, 0, 0},
    {BUTTON3_PIN, false, 0, 0},
    {BUTTON4_PIN, false, 0, 0}
};

// Ponteiros para funções de callback (inicializados com padrões)
void default_button_pressed(uint8_t pin) {
    printf("Botão %d pressionado (callback padrão)\n", pin);
}
void default_button_released(uint8_t pin) {
    printf("Botão %d liberado (callback padrão)\n", pin);
}
void default_button_short_press(uint8_t pin) {
    printf("Botão %d pressionado brevemente (callback padrão)\n", pin);
}
void default_button_long_press(uint8_t pin) {
    printf("Botão %d pressionado por muito tempo (callback padrão)\n", pin);
}

// Inicializa os ponteiros com as funções padrão
void (*on_button_pressed)(uint8_t pin) = default_button_pressed;
void (*on_button_released)(uint8_t pin) = default_button_released;
void (*on_button_short_press)(uint8_t pin) = default_button_short_press;
void (*on_button_long_press)(uint8_t pin) = default_button_long_press;

// Função para verificar o estado dos botões e detectar long press
bool check_long_press(struct repeating_timer *t) {
    for (int i = 0; i < 4; i++) {
        if (buttons[i].pressed) {
            absolute_time_t now = get_absolute_time();
            int64_t press_duration = absolute_time_diff_us(buttons[i].press_time, now);
            if (press_duration >= LONG_PRESS_TIME_US && !buttons[i].long_press_detected) {
                on_button_long_press(buttons[i].pin);
                buttons[i].long_press_detected = true;
            }
        }
    }
    return true;
}

// Callback GPIO para gerenciar os eventos
void gpio_callback(uint gpio, uint32_t events) {
    for (int i = 0; i < 4; i++) {
        if (buttons[i].pin == gpio) {
            absolute_time_t now = get_absolute_time();

            // Verifica diretamente o estado atual do GPIO
            if (gpio_get(gpio) == 0) { // Botão está pressionado (LOW)
                int64_t time_since_release = absolute_time_diff_us(buttons[i].last_release_time, now);
                if (time_since_release > DEBOUNCE_TIME_US) { // Ignora eventos dentro do delay
                    if (!buttons[i].pressed) { // Apenas registra se ainda não foi detectado
                        buttons[i].pressed = true;
                        buttons[i].press_time = now;
                        buttons[i].long_press_detected = false; // Redefine o estado de long press
                        on_button_pressed(gpio);
                    }
                }
            } else { // Botão está liberado (HIGH)
                if (buttons[i].pressed) { // Apenas registra se estava pressionado
                    buttons[i].pressed = false;

                    // Calcula o tempo de pressionamento
                    int64_t press_duration = absolute_time_diff_us(buttons[i].press_time, now);

                    // Verifica short press
                    if (!buttons[i].long_press_detected && press_duration < LONG_PRESS_TIME_US) {
                        on_button_short_press(gpio);
                    }

                    on_button_released(gpio);

                    // Registra o momento do release
                    buttons[i].last_release_time = now;
                }
            }
        }
    }
}

// Inicializa os botões e configura as interrupções
void init_buttons(void) {
    for (int i = 0; i < 4; i++) {
        gpio_init(buttons[i].pin); // Inicializa o GPIO
        gpio_set_dir(buttons[i].pin, GPIO_IN); // Define como entrada
        gpio_pull_up(buttons[i].pin); // Habilita o resistor de pull-up interno
        gpio_set_irq_enabled_with_callback(
            buttons[i].pin,
            GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
            true,
            gpio_callback
        ); // Ativa interrupções
    }

    // Inicializa o temporizador periódico
    static repeating_timer_t timer;
    add_repeating_timer_ms(100, check_long_press, NULL, &timer);
}