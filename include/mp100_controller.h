//
// Created by paulp on 26/04/2025.
//

#ifndef AMPERO_CONTROLLER_H
#define AMPERO_CONTROLLER_H

#include <stdint.h>

#define MP100_SYSEX_TEMPO_ID {33, 37, 127, 77, 80, 45, 80, 18, 0, 2, 0, 32}
#define MP100_SYSEX_PATCH_ID {33, 37, 127, 77, 80, 45, 80, 18, 0, 2, 6, 4}


int mp100_sysex_tempo(const uint8_t *data, uint32_t length);

int mp100_sysex_patch(const uint8_t *data, uint32_t length);

int* mp100_bpm_to_cc(int bpm);

#endif //AMPERO_CONTROLLER_H
