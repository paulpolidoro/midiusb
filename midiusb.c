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

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#ifdef RASPBERRYPI_PICO_W
// The Board LED is controlled by the CYW43 WiFi/Bluetooth module
#include "pico/cyw43_arch.h"
#endif

TapTempo tempo;

static uint8_t midi_dev_addr = 0;

static void blink_led(void)
{
    static absolute_time_t previous_timestamp = {0};

    static bool led_state = false;

    absolute_time_t now = get_absolute_time();
    
    int64_t diff = absolute_time_diff_us(previous_timestamp, now);
    if (diff > 1000000) {
#ifdef RASPBERRYPI_PICO_W
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
#else
        board_led_write(led_state);
#endif
        led_state = !led_state;
        previous_timestamp = now;
    }
}

static void send_next_note(void)
{
    static uint8_t first_note = 0x5b; // Mackie Control rewind
    static uint8_t last_note = 0x5f; // Mackie Control stop
    static uint8_t message[6] = {
        0x90, 0x5f, 0x00,
        0x90, 0x5b, 0x7f,
    };
    // toggle NOTE On, Note Off for the Mackie Control channels 1-8 REC LED
    const uint32_t interval_ms = 1000;
    static uint32_t start_ms = 0;

    // Blink every interval ms
    if ( board_millis() - start_ms < interval_ms) {
        return; // not enough time
    }
    start_ms += interval_ms;

    uint32_t nwritten = 0;
    // Transmit the note message on the highest cable number
    uint8_t cable = tuh_midih_get_num_tx_cables(midi_dev_addr) - 1;
    nwritten = 0;
    nwritten += tuh_midi_stream_write(midi_dev_addr, cable, message, sizeof(message));
 
    if (nwritten != 0)
    {
        ++message[1];
        ++message[4];
        if (message[1] > last_note)
            message[1] = first_note;
        if (message[4] > last_note)
            message[4] = first_note;
    }
}

void send_cc(uint8_t dev_addr, uint8_t cable_num, uint8_t channel, uint8_t cc_number, uint8_t cc_value) {
    // Status byte para Control Change no canal especificado
    uint8_t status_byte = 0xB0 | (channel & 0x0F);
    
    // Buffer contendo a mensagem MIDI CC
    uint8_t midi_cc_message[] = {status_byte, cc_number, cc_value};
    
    // Enfileirar a mensagem MIDI CC
    tuh_midi_stream_write(dev_addr, cable_num, midi_cc_message, sizeof(midi_cc_message));
    
    // Enviar a mensagem enfileirada
    tuh_midi_stream_flush(dev_addr);
}

int bpm_to_ms(int bpm) {
  if (bpm <= 0) {
      return 0; // Retorna -1 para valores de BPM inválidos
  }
  return 60000 / bpm; // Converte BPM para milissegundos
}

void custom_button_pressed(uint8_t pin) {
  uint16_t ultimo_bpm = get_bpm(&tempo); 

  tap(&tempo);

  if(ultimo_bpm != get_bpm(&tempo)) {
    if(get_bpm(&tempo)  >= MIN_BPM){
      led_blink(LEDTAP_PIN, bpm_to_ms(get_bpm(&tempo)) / 2); // Pisca o LED de tap tempo
    }
  } 
}

void custom_button_released(uint8_t pin) {
}

void custom_button_short_press(uint8_t pin) {
}

void custom_button_long_press(uint8_t pin) {
  led_off(LEDTAP_PIN); 
}


int main() {
  uart_init(UART_ID, BAUD_RATE);

  // Configura os pinos TX e RX
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Configura o formato da UART (8 bits de dados, sem paridade, 1 bit de parada)
  uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

  // Habilita a UART
  uart_set_fifo_enabled(UART_ID, true);

  bi_decl(bi_program_description("A USB MIDI host example."));

  board_init();

  // Sobrescreve os callbacks padrão
  on_button_pressed = custom_button_pressed;
  on_button_released = custom_button_released;
  on_button_short_press = custom_button_short_press;
  on_button_long_press = custom_button_long_press;

  init_buttons();

  init_tap_tempo(&tempo);

  init_leds();

  led_on(LEDPOWER_PIN); // Liga o LED de power

  uart_puts(UART_ID, "MIDI Conntroller\r\n");
  tusb_init();

#ifdef RASPBERRYPI_PICO_W
    // for the LED blink
    if (cyw43_arch_init()) {
        printf("WiFi/Bluetooh module init for LED blink failed");
        return -1;
    }
#endif
    while (1) {
        tuh_task();

        // blink_led();
        bool connected = midi_dev_addr != 0 && tuh_midi_configured(midi_dev_addr);

        // device must be attached and have at least one endpoint ready to receive a message
        if (connected && tuh_midih_get_num_tx_cables(midi_dev_addr) >= 1) {
            send_next_note();
            // transmit any previously queued bytes (do this once per main loop)
            tuh_midi_stream_flush(midi_dev_addr);
        }
    }
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_midi_mount_cb(uint8_t dev_addr, uint8_t in_ep, uint8_t out_ep, uint8_t num_cables_rx, uint16_t num_cables_tx)
{
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "MIDI device address = %d, IN endpoint %d has %d cables, OUT endpoint %d has %d cables\r\n",
      dev_addr, in_ep & 0xf, num_cables_rx, out_ep & 0xf, num_cables_tx);
  uart_puts(UART_ID, buffer);


  if (midi_dev_addr == 0) {
    // then no MIDI device is currently connected
    midi_dev_addr = dev_addr;
  }
  else {

    uart_puts(UART_ID, "A different USB MIDI Device is already connected.\r\nOnly one device at a time is supported in this program\r\nDevice is disabled\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_midi_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  char buffer[64];

  if (dev_addr == midi_dev_addr) {
    midi_dev_addr = 0;

    snprintf(buffer, sizeof(buffer), "MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  }
  else {
    snprintf(buffer, sizeof(buffer), "MIDI device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  }

  uart_puts(UART_ID, buffer);
}

void tuh_midi_rx_cb(uint8_t dev_addr, uint32_t num_packets)
{
  if (midi_dev_addr == dev_addr) {
    if (num_packets != 0) {
      uint8_t cable_num;
      uint8_t buffer[48];
      char buffertxt[64];
      while (1) {
        uint32_t bytes_read = tuh_midi_stream_read(dev_addr, &cable_num, buffer, sizeof(buffer));
        if (bytes_read == 0)
          return;
          
          snprintf(buffertxt, sizeof(buffertxt), "MIDI RX Cable #%u:", cable_num);
          uart_puts(UART_ID, buffertxt);
        for (uint32_t idx = 0; idx < bytes_read; idx++) {
          snprintf(buffertxt, sizeof(buffertxt), "%02x ", buffer[idx]);
          uart_puts(UART_ID, buffertxt);
        }
        uart_puts(UART_ID, "\r\n");
      }
    }
  }
}

void tuh_midi_tx_cb(uint8_t dev_addr)
{
    (void)dev_addr;
}