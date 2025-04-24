#include "led_controller.h"

void init_leds(void) {
    gpio_init(LEDPOWER_PIN);
    gpio_set_dir(LEDPOWER_PIN, GPIO_OUT);

    gpio_init(LEDTAP_PIN);
    gpio_set_dir(LEDTAP_PIN, GPIO_OUT);

    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);

    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);

    gpio_init(LED3_PIN);
    gpio_set_dir(LED3_PIN, GPIO_OUT);

    gpio_init(LED4_PIN);
    gpio_set_dir(LED4_PIN, GPIO_OUT);
}

void on(uint pin) {
    gpio_put(pin, 1);
}

void off(uint pin) {
    gpio_put(pin, 0);
}

void blink(uint pin, uint interval_ms) {
    on(pin);
    sleep_ms(interval_ms);
    off(pin);
    sleep_ms(interval_ms);
}
