#include "led_controller.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include <stdio.h>


LEDState led_states[] = {
    {LEDPOWER_PIN, false, {}, 500, false},
    {LEDTAP_PIN, false, {}, 500, false},
    {LED1_PIN, false, {}, 500, false},
    {LED2_PIN, false, {}, 500, false},
    {LED3_PIN, false, {}, 500, false},
    {LED4_PIN, false, {}, 500, false}
};


/**
 * Inicia os LEDS
 */
void init_leds() {
    for (uint i = 0; i < sizeof(led_states) / sizeof(LEDState); i++) {
        gpio_init(led_states[i].pin);
        gpio_set_dir(led_states[i].pin, GPIO_OUT);
        gpio_put(led_states[i].pin, 0);
    }
}

/**
 * Procura o LED pelo PIN recebido
 * @param pin
 * @return
 */
LEDState *find_led(const int pin) {
    for (uint i = 0; i < sizeof(led_states) / sizeof(LEDState); i++) {
        if (led_states[i].pin == pin) {
            return &led_states[i];
        }
    }

    return NULL;
}

/**
 * Liga o LED, interrompe o blink se estiver ativo
 * @param pin
 */
void led_on(const int pin) {
    LEDState *led = find_led(pin);

    if (led != NULL) {
        printf("LED %d ON\r\n", pin);

        led->is_blinking = false;
        cancel_repeating_timer(&led->timer);
        gpio_put(pin, 1);
        led->led_state = true;
    }
}

/**
 * Desliga o LED, interrompe o blink se estiver ativo
 * @param pin
 */
void led_off(const int pin) {
    LEDState *led = find_led(pin);

    if (led != NULL) {
        printf("LED %d OFF\r\n", pin);

        led->is_blinking = false;
        cancel_repeating_timer(&led->timer);
        gpio_put(pin, 0);
        led->led_state = false;
    }
}

/**
 * Liga ou desliga o LED de acordo com o status atual
 * @param pin
 */
void led_toggle(const int pin) {
    const LEDState *led = find_led(pin);

    if (led != NULL) {
        printf("LED %d TOGGLE\r\n", pin);

        if (led->is_blinking || led->led_state) {
            led_off(pin);
        } else {
            led_on(pin);
        }
    }
}

/**
 * Callback apra executar as alterações de blink do LED
 * @param rt
 * @return
 */
_Bool blink_callback(const repeating_timer_t *rt) {
    LEDState *led = rt->user_data;

    if (led->is_blinking) {
        // Alterna o estado do LED
        led->led_state = !led->led_state;
        gpio_put(led->pin, led->led_state);

        // Se há timeout configurado, atualiza o tempo decorrido
        if (led->timeout_ms > 0) {
            led->elapsed_ms += led->interval_ms;

            if (led->elapsed_ms >= led->timeout_ms) {
                // Cancela o blinking
                led->is_blinking = false;
                gpio_put(led->pin, led->before_blink);  // Garante que o LED apague
                return false;              // Cancela o timer
            }
        }

        return true; // Continua o blinking
    }

    return false; // Cancela o timer se não estiver piscando
}

/**
 * Inicia o blink ou altera a velocidade caso já esteja iniciado
 * @param pin
 * @param interval_ms
 * @param timeout_ms
 */
void led_blink(const int pin, const int interval_ms, const int timeout_ms) {
    printf("LED %d BLINK \r\n", pin);
    for (uint i = 0; i < sizeof(led_states) / sizeof(LEDState); i++) {
        if (led_states[i].pin == pin) {
            // Cancela o timer existente, se necessário
            if (led_states[i].is_blinking) {
                if (led_states[i].interval_ms == interval_ms && led_states[i].timeout_ms == timeout_ms) {
                    return; // Já está piscando com os mesmos parâmetros
                }
                cancel_repeating_timer(&led_states[i].timer);
            }

            // Configura os novos valores
            led_states[i].is_blinking = true;
            led_states[i].interval_ms = interval_ms;
            led_states[i].timeout_ms = timeout_ms;
            led_states[i].elapsed_ms = 0;
            led_states[i].before_blink = led_states[i].led_state;

            // Inicia o blinking
            add_repeating_timer_ms(interval_ms, blink_callback, &led_states[i], &led_states[i].timer);
            break;
        }
    }
}
