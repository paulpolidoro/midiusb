//
// Created by paulp on 26/04/2025.
//

#include "mp100_controller.h"
#include <stdio.h>   // Para printf
#include <string.h>  // Para memcmp ou manipulação de strings

/**
 * Converte BPM Sysex para Inteiro
 * @param data1
 * @param data2
 * @return
 */
int convert_to_bpm(const int data1, const int data2) {
    if (data1 == 0) {
        return data2;
    }

    if (data1 == 1) {
        return data2 + 128;
    }

    return -1;
}

/**
 * Verifica se o Sysex é Tempo, se for converte e retorna o BPM
 * @param data
 * @param length
 * @return
 */
int mp100_sysex_tempo(const uint8_t *data, const uint32_t length) {
    const uint8_t target_sequence[] = MP100_SYSEX_TEMPO_ID;
    const uint32_t target_length = sizeof(target_sequence) / sizeof(target_sequence[0]);

    if (length >= target_length) {
        for (uint32_t i = 0; i <= length - target_length; i++) {
            if (memcmp(&data[i], target_sequence, target_length) == 0) {
                return convert_to_bpm(data[i + length - 2], data[i + length - 1]);
            }
        }
    }

    return -1;
}

/**
 * Verifica se o Sysex é alteração de patch, se for retorna o patch atual
 * @param data
 * @param length
 * @return
 */
int mp100_sysex_patch(const uint8_t *data, const uint32_t length) {
    const uint8_t target_sequence[] = MP100_SYSEX_PATCH_ID;
    const uint32_t target_length = sizeof(target_sequence) / sizeof(target_sequence[0]);

    if (length >= target_length) {
        for (uint32_t i = 0; i <= length - target_length; i++) {
            if (memcmp(&data[i], target_sequence, target_length) == 0) {
                printf("PATCH CHANGE TO %d", data[i + length - 1]);
                return data[i + length - 1] -1;
            }
        }
    }

    return -1;
}

/**
 * Converte um valor inteiro de BPM para um array com valores de CC
 * @param bpm
 * @return
 */
int* mp100_bpm_to_cc(const int bpm) {
    static int result[2]; // Array estático para retorno

    if (bpm <= 127) {
        result[0] = 0;        // Código 0 para intervalo 40-127
        result[1] = bpm;      // BPM original
    } else {
        result[0] = 1;        // Código 1 para intervalo 128-250
        result[1] = bpm - 128;
    }// BPM ajustado (128 = 0, 129 = 1, etc.)

    return result;
}


