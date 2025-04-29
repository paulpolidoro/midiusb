#include <stdio.h>
#include "midi_controller.h"
#include "tusb.h"
#include "usb_midi_host.h"

/**
 * Envia uma mensagem via USB
 * @param dev_addr
 * @param cable_num
 * @param midi_message
 * @param length
 */
void usb_send(const uint8_t dev_addr, const uint8_t cable_num, const uint8_t* midi_message, const size_t length) {
    tuh_midi_stream_write(dev_addr, cable_num, midi_message, length);

    tuh_midi_stream_flush(dev_addr);
}

/**
 * Prepara e envia uma mensagem CC via USB
 * @param dev_addr
 * @param cable_num
 * @param channel
 * @param cc_number
 * @param cc_value
 */
void usb_send_cc(const uint8_t dev_addr, const uint8_t cable_num, const uint8_t channel, const uint8_t cc_number, const uint8_t cc_value) {
    const uint8_t midi_cc_message[] = {(MIDI_CC_START | (channel & 0x0F)), cc_number, cc_value};

    usb_send(dev_addr, cable_num, midi_cc_message, sizeof(midi_cc_message));
}

/**
 * Prepara e envia uma mensagem PC via USB
 * @param dev_addr
 * @param cable_num
 * @param channel
 * @param program_number
 */
void usb_send_pc(const uint8_t dev_addr, const uint8_t cable_num, const uint8_t channel, const uint8_t program_number) {
    const uint8_t midi_pc_message[] = {(MIDI_PC_START | (channel & 0x0F)), program_number};

    usb_send(dev_addr, cable_num, midi_pc_message, sizeof(midi_pc_message));
}

/**
 * Prepara e envia uma mensagem SysEx via USB
 * @param dev_addr
 * @param cable_num
 * @param sysex_data
 * @param length
 */
void usb_send_sysex(const uint8_t dev_addr, const uint8_t cable_num, const uint8_t* sysex_data, const size_t length) {
    uint8_t midi_sysex_message[length + 2];

    midi_sysex_message[0] = MIDI_SYSEX_START;

    memcpy(&midi_sysex_message[1], sysex_data, length);

    midi_sysex_message[length + 1] = MIDI_SYSEX_END;

    usb_send(dev_addr, cable_num, midi_sysex_message, sizeof(midi_sysex_message));
}