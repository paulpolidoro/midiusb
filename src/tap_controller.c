#include <stdio.h>
#include "tap_controller.h"
#include "led_controller.h"

/**
 *
 * @param tempo
 * @param bpm
 */
void set_tempo(TapTempo *tempo, int bpm) {
    if (bpm < 0) {
        return;
    }

    if (bpm < MIN_BPM) {
        bpm = MIN_BPM;
    } else if (bpm > MAX_BPM) {
        bpm = MAX_BPM;
    }

    tempo->previous_bpm = tempo->bpm;
    tempo->previous_ms = tempo->ms;

    tempo->bpm = bpm;
    tempo->ms = 60000 / bpm;

    led_blink(LEDTAP_PIN, tempo->ms / 2, 0);

    printf("BPM: %d \r\n", tempo->bpm);
}

/**
 *
 * @param tempo
 */
void tap(TapTempo *tempo) {
    const absolute_time_t now = get_absolute_time();

    if (tempo->valid_tap) {
        const uint16_t bpm = 60000000 / absolute_time_diff_us(tempo->last_tap_time, now);

        if (bpm < RESET_BPM_THRESHOLD) {
            tempo->valid_tap = true;
        } else {
            set_tempo(tempo, bpm);
        }
    } else {
        tempo->valid_tap = true;
    }

    tempo->last_tap_time = now;
}

/**
 *
 * @param tempo
 */
void init_tap_tempo(TapTempo *tempo) {
    tempo->last_tap_time = nil_time;
    tempo->previous_bpm = 0;
    tempo->bpm = 60;
    tempo->previous_ms = 0;
    tempo->ms = 1000;
    tempo->valid_tap = false;
}
