#ifndef TAP_CONTROLLER_H
#define TAP_CONTROLLER_H

#include <stdint.h>
#include "pico/time.h"

// Limites do BPM
#define MIN_BPM 40  // BPM mínimo válido
#define MAX_BPM 250 // BPM máximo permitido
#define RESET_BPM_THRESHOLD 20 // BPM abaixo do qual ocorre o reset

// Estrutura para armazenar informações do tap tempo
typedef struct {
    absolute_time_t last_tap_time; // Momento do último tap
    uint16_t bpm;                 // BPM calculado
    bool valid_tap;               // Indica se o último tap foi válido
} TapTempo;

// Função para inicializar o tap tempo
void init_tap_tempo(TapTempo *tempo);

// Função para registrar um tap
void tap(TapTempo *tempo);

// Função para obter o BPM atual
uint16_t get_bpm(const TapTempo *tempo);

#endif // TAP_CONTROLLER_H