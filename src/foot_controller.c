#include "foot_controller.h"

// Definir os pinos dos botões
ButtonState buttons[] = {
    {BUTTON1_PIN, false, 0},
    {BUTTON2_PIN, false, 0},
    {BUTTON3_PIN, false, 0},
    {BUTTON4_PIN, false, 0}
};

// Funções de callback
void on_button_pressed(uint8_t pin) {
    printf("Button %d pressed\n", pin);
}

void on_button_released(uint8_t pin) {
    printf("Button %d released\n", pin);
}

void on_button_short_press(uint8_t pin) {
    printf("Button %d short press\n", pin);
}

void on_button_long_press(uint8_t pin) {
    printf("Button %d long press\n", pin);
}

void on_buttons_combination(uint8_t pin1, uint8_t pin2) {
    printf("Buttons %d and %d pressed simultaneously\n", pin1, pin2);
}

// Função de interrupção GPIO
void gpio_callback(uint gpio, uint32_t events) {
    bool combination_detected = false;

    // Verificar combinações de botões pressionados
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (buttons[i].pressed && buttons[j].pressed) {
                on_buttons_combination(buttons[i].pin, buttons[j].pin);
                combination_detected = true;
            }
        }
    }

    // Executar callbacks individuais apenas se nenhuma combinação for detectada
    if (!combination_detected) {
        for (int i = 0; i < 4; i++) {
            if (buttons[i].pin == gpio) {
                if (events & GPIO_IRQ_EDGE_RISE) {
                    buttons[i].pressed = false;
                    absolute_time_t now = get_absolute_time();
                    int64_t press_duration = absolute_time_diff_us(buttons[i].press_time, now);
                    on_button_released(gpio);
                    if (press_duration < 2000000) {
                        on_button_short_press(gpio);
                    } else {
                        on_button_long_press(gpio);
                    }
                } else if (events & GPIO_IRQ_EDGE_FALL) {
                    buttons[i].pressed = true;
                    buttons[i].press_time = get_absolute_time();
                    on_button_pressed(gpio);
                }
            }
        }
    }
}

// Inicializar os botões
void init_buttons(void) {
    for (int i = 0; i < 4; i++) {
        gpio_init(buttons[i].pin);
        gpio_set_dir(buttons[i].pin, GPIO_IN);
        gpio_pull_up(buttons[i].pin);
        gpio_set_irq_enabled_with_callback(buttons[i].pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    }
}
