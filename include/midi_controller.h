#ifndef MIDI_CONTROLLER_H
#define MIDI_CONTROLLER_H

#include <stdio.h>

/**
 * Definição de mensagens MIDI
 */
#define MAX_SYSEX_SIZE 256
#define MIDI_CC_START 0xB0
#define MIDI_PC_START 0xC0
#define MIDI_SYSEX_START 0xF0
#define MIDI_SYSEX_END 0xF7

/**
 * Prepara e envia uma mensagem CC via USB
 * @param dev_addr
 * @param cable_num
 * @param channel
 * @param cc_number
 * @param cc_value
 */
void usb_send_cc(uint8_t dev_addr, uint8_t cable_num, uint8_t channel, uint8_t cc_number, uint8_t cc_value);

/**
 * Prepara e envia uma mensagem PC via USB
 * @param dev_addr
 * @param cable_num
 * @param channel
 * @param program_number
 */
void usb_send_pc(uint8_t dev_addr, uint8_t cable_num, uint8_t channel, uint8_t program_number);

/**
 * Prepara e envia uma mensagem SysEx via USB
 * @param dev_addr
 * @param cable_num
 * @param sysex_data
 * @param length
 */
void usb_send_sysex(uint8_t dev_addr, uint8_t cable_num, uint8_t* sysex_data, size_t length);

#endif //MIDI_CONTROLLER_H
