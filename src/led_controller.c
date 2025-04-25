#include "led_controller.h"
#include "pico/time.h"
#include "hardware/timer.h"

// Estrutura para gerenciar o estado de cada LED
typedef struct {
    uint pin;                     // Pino associado ao LED
    bool is_blinking;             // Indica se o LED está piscando
    repeating_timer_t timer;      // Temporizador para alternar o estado
    uint interval_ms;             // Intervalo atual em ms
    bool led_state;               // Estado atual do LED (on/off)
} LEDState;

// Variáveis para gerenciar os estados dos LEDs
LEDState led_states[] = {
    {LEDPOWER_PIN, false, {}, 500, false},
    {LEDTAP_PIN, false, {}, 500, false},
    {LED1_PIN, false, {}, 500, false},
    {LED2_PIN, false, {}, 500, false},
    {LED3_PIN, false, {}, 500, false},
    {LED4_PIN, false, {}, 500, false}
};

// Inicializa os LEDs configurando os pinos correspondentes
void init_leds(void) {
    for (int i = 0; i < sizeof(led_states) / sizeof(LEDState); i++) {
        gpio_init(led_states[i].pin);
        gpio_set_dir(led_states[i].pin, GPIO_OUT);
        gpio_put(led_states[i].pin, 0); // Certifica-se de que os LEDs começam desligados
    }
}

// Liga o LED correspondente ao pino e para o piscar
void led_on(uint pin) {
    for (int i = 0; i < sizeof(led_states) / sizeof(LEDState); i++) {
        if (led_states[i].pin == pin) {
            led_states[i].is_blinking = false; // Para o piscar
            cancel_repeating_timer(&led_states[i].timer); // Cancela o temporizador
            gpio_put(pin, 1);                 // Liga o LED
            led_states[i].led_state = true;   // Atualiza o estado do LED
            break;
        }
    }
}

// Desliga o LED correspondente ao pino e para o piscar
void led_off(uint pin) {
    for (int i = 0; i < sizeof(led_states) / sizeof(LEDState); i++) {
        if (led_states[i].pin == pin) {
            led_states[i].is_blinking = false; // Para o piscar
            cancel_repeating_timer(&led_states[i].timer); // Cancela o temporizador
            gpio_put(pin, 0);                 // Desliga o LED
            led_states[i].led_state = false;  // Atualiza o estado do LED
            break;
        }
    }
}

// Callback usado pelo temporizador para alternar o estado do LED
bool blink_callback(repeating_timer_t *rt) {
    LEDState *led = (LEDState *)rt->user_data;

    if (led->is_blinking) {
        // Alterna o estado do LED
        led->led_state = !(led->led_state);
        gpio_put(led->pin, led->led_state);
        return true; // Continua repetindo o temporizador
    }

    return false; // Para o temporizador
}

// Função de piscar para cada LED (atualiza velocidade se já estiver piscando)
void led_blink(uint pin, uint interval_ms) {
    for (int i = 0; i < sizeof(led_states) / sizeof(LEDState); i++) {
        if (led_states[i].pin == pin) {
            // Se já estiver piscando, atualiza o intervalo
            if (led_states[i].is_blinking) {
                // Cancela o temporizador atual de forma segura
                if (cancel_repeating_timer(&led_states[i].timer)) {
                    printf("Temporizador cancelado com sucesso para o pino %d\n", pin);
                } else {
                    printf("Erro ao cancelar temporizador para o pino %d\n", pin);
                }
            }

            // Atualiza o estado e recria o temporizador
            led_states[i].is_blinking = true;
            led_states[i].interval_ms = interval_ms; // Atualiza o intervalo
            if (add_repeating_timer_ms(interval_ms, blink_callback, &led_states[i], &led_states[i].timer)) {
                printf("Novo temporizador criado para o pino %d com intervalo de %d ms\n", pin, interval_ms);
            } else {
                printf("Falha ao criar temporizador para o pino %d\n", pin);
            }

            break;
        }
    }
}