#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_midi_host.h"
#include "foot_controller.h"
#include "tap_controller.h"
#include "led_controller.h"
#include "mp100_controller.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define MAX_SYSEX_SIZE 256

uint8_t sysex_data[MAX_SYSEX_SIZE];
uint32_t sysex_index = 0;
TapTempo tempo;
int global_tempo = false;

static uint8_t midi_dev_addr = 0;



void send_cc(const uint8_t dev_addr, const uint8_t cable_num, const uint8_t channel, const uint8_t cc_number, const uint8_t cc_value) {
    // Status byte para Control Change no canal especificado
    const uint8_t status_byte = 0xB0 | (channel & 0x0F);

    // Buffer contendo a mensagem MIDI CC
    const uint8_t midi_cc_message[] = {status_byte, cc_number, cc_value};

    // Enfileirar a mensagem MIDI CC
    tuh_midi_stream_write(dev_addr, cable_num, midi_cc_message, sizeof(midi_cc_message));

    // Enviar a mensagem enfileirada
    tuh_midi_stream_flush(dev_addr);
}

void send_tempo() {
    const int* bpm_cc = mp100_bpm_to_cc(tempo.bpm);

    send_cc(midi_dev_addr, 0, 1, 73, bpm_cc[0]);
    send_cc(midi_dev_addr, 0, 1, 74, bpm_cc[1]);
}

void patch_change(const int cc, const int pin) {
    led_blink(pin, 100, 500);
    send_cc(midi_dev_addr, 0, 1, cc, 127);
    if (global_tempo) {
        send_tempo();
    }
}

void foot_pressed(const uint8_t pin) {
    switch (pin) {
        case FOOT1_PIN:
            printf("BANK UP \r\n");
            patch_change(25, LED1_PIN);

            break;
        case FOOT2_PIN:
            printf("BANK DOWN \r\n");
            patch_change(24, LED2_PIN);

            break;
        case FOOT3_PIN:
            tap(&tempo);

            if (tempo.previous_bpm != tempo.bpm) {
                send_tempo();
            }
            break;
        case FOOT4_PIN:

            break;
        default: ;
            break;
    }
}

void foot_released(uint8_t pin) {
}

void foot_short_press(const uint8_t pin) {
    switch (pin) {
        case FOOT3_PIN:
            break;
        case FOOT4_PIN:
            printf("CONTROL \r\n");
            send_cc(midi_dev_addr, 0, 1, 72, 127);
            led_toggle(LED4_PIN);
        default:
            break;
    }
}

void foot_long_press(const uint8_t pin) {
    switch (pin) {
        case FOOT3_PIN:
            led_off(LEDTAP_PIN);
        case FOOT4_PIN:
            global_tempo = !global_tempo;
            led_blink(LED4_PIN, 100, 1000);
        default:
        break;
    }
}

void init_uart() {
    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);


    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

    uart_set_fifo_enabled(UART_ID, true);

    printf("MIDI Conntroller\r\n");
}

int main() {
    stdio_init_all();
    init_uart();
    init_foots();
    init_leds();

    init_tap_tempo(&tempo);

    led_on(LEDPOWER_PIN);

    board_init();

    bi_decl(bi_program_description("A USB MIDI host example."));

    on_foot_pressed = foot_pressed;
    on_foot_released = foot_released;
    on_foot_short_press = foot_short_press;
    on_foot_long_press = foot_long_press;

    tusb_init();

    while (1) {
        tuh_task();

        bool const connected = midi_dev_addr != 0 && tuh_midi_configured(midi_dev_addr);

        // device must be attached and have at least one endpoint ready to receive a message
        //if (connected && tuh_midih_get_num_tx_cables(midi_dev_addr) >= 1) {
            //send_next_note();
            // transmit any previously queued bytes (do this once per main loop)
            //tuh_midi_stream_flush(midi_dev_addr);
        //}
    }
}

void tuh_midi_mount_cb(const uint8_t dev_addr, const uint8_t in_ep, const uint8_t out_ep, const uint8_t num_cables_rx, const uint16_t num_cables_tx) {
    printf("MIDI device address = %d, IN endpoint %d has %d cables, OUT endpoint %d has %d cables\r\n",
           dev_addr, in_ep & 0xf, num_cables_rx, out_ep & 0xf, num_cables_tx);

    if (midi_dev_addr == 0) {
        midi_dev_addr = dev_addr;
    } else {
        printf(
            "A different USB MIDI Device is already connected.\r\nOnly one device at a time is supported in this program\r\nDevice is disabled\r\n");
    }
}

void tuh_midi_umount_cb(const uint8_t dev_addr, const uint8_t instance) {
    if (dev_addr == midi_dev_addr) {
        midi_dev_addr = 0;

        printf("MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
    } else {
        printf("MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
    }
}

void tuh_midi_rx_cb(const uint8_t dev_addr, const uint32_t num_packets) {
    if (midi_dev_addr == dev_addr && num_packets != 0) {
        uint8_t cable_num;
        uint8_t buffer[48];

        while (1) {
            const uint32_t bytes_read = tuh_midi_stream_read(dev_addr, &cable_num, buffer, sizeof(buffer));
            if (bytes_read == 0) {
                return;
            }

            for (uint32_t idx = 0; idx < bytes_read; idx++) {
                const uint8_t byte = buffer[idx];

                if (byte == 0xF0) { // Início de SysEx
                    sysex_index = 0; // Reinicia o índice
                    continue;
                }

                if (byte == 0xF7) { // Fim de SysEx
                    if (sysex_index > 0) { // Processa o SysEx se houver dados
                        set_tempo(&tempo, mp100_sysex_tempo(sysex_data, sysex_index));
                        mp100_sysex_patch(sysex_data, sysex_index);

                        sysex_index = 0; // Reinicia para o próximo SysEx
                    }
                    continue;
                }

                // Adiciona o byte ao array, se houver espaço
                if (sysex_index < MAX_SYSEX_SIZE) {
                    sysex_data[sysex_index++] = byte;
                } else {
                    printf("Erro: SysEx excedeu o tamanho máximo!\n");
                }
            }
        }
    }
}
