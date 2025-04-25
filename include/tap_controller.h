#ifndef TAP_CONTROLLER_H
#define TAP_CONTROLLER_H

#include <stdint.h>
#include "pico/time.h"


#define MIN_BPM 40
#define MAX_BPM 250
#define RESET_BPM_THRESHOLD 20


typedef struct {
    absolute_time_t last_tap_time;
    int previous_bpm;
    int bpm;
    int previous_ms;
    int ms;
    bool valid_tap;
} TapTempo;


/**
 * Inicia o processamento de tap tempo
 * @param tempo
 */
void init_tap_tempo(TapTempo *tempo);

/**
 * Seta um novo tap
 * @param tempo
 */
void tap(TapTempo *tempo);

#endif
