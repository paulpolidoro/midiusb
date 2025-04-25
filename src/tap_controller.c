#include "tap_controller.h"

// Função para inicializar o tap tempo
void init_tap_tempo(TapTempo *tempo) {
    tempo->last_tap_time = nil_time; // Inicializa com um tempo inválido
    tempo->bpm = 0;                 // Inicializa o BPM com 0
    tempo->valid_tap = false;       // Marca como inválido até um tap ser registrado
}

// Função para registrar um tap
void tap(TapTempo *tempo) {
    absolute_time_t now = get_absolute_time(); // Obtém o tempo atual

    if (tempo->valid_tap) {
        // Calcula a diferença de tempo entre os taps (em microssegundos)
        int64_t time_diff_us = absolute_time_diff_us(tempo->last_tap_time, now);

        // Converte a diferença para BPM
        uint16_t bpm_calculated = 60000000 / time_diff_us; // Fórmula: BPM = 60 / (tempo_em_segundos)

        // Verifica os limites do BPM
        if (bpm_calculated < RESET_BPM_THRESHOLD) {
            // Reseta o tap tempo e conta como primeiro tap
            tempo->valid_tap = true;
            tempo->bpm = 0;
            printf("Tap resetado.\n");
        } else {
            // Ajusta o BPM dentro dos limites definidos
            if (bpm_calculated < MIN_BPM) {
                bpm_calculated = MIN_BPM;
            } else if (bpm_calculated > MAX_BPM) {
                bpm_calculated = MAX_BPM;
            }

            // Salva o BPM calculado
            tempo->bpm = bpm_calculated;
            printf("BPM registrado: %d\n", tempo->bpm);
        }
    } else {
        // Primeiro tap, valida e registra o tempo
        tempo->valid_tap = true;
        printf("Primeiro tap registrado.\n");
    }

    // Atualiza o tempo do último tap
    tempo->last_tap_time = now;
}

// Função para obter o BPM atual
uint16_t get_bpm(const TapTempo *tempo) {
    return tempo->bpm; // Retorna o BPM registrado
}