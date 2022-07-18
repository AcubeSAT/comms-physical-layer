#include <oqpsk.h>
#include <iostream>
#include "filter.h"

#define NUM_OQPSK_TAPS 23

double oqpsk_taps[] = {-0.0020520018879324198, -0.0003224409883841872, 0.002508002333343029, -0.00506692985072732, 0.005374290514737368,
                       0.0015200789785012603, -0.007524007000029087, 0.021281106397509575, -0.03762003779411316, -0.05320276319980621, 0.2901403307914734,
                       0.5699287056922913, 0.2901403307914734, -0.05320276319980621, -0.03762003779411316, 0.021281106397509575,
                       -0.007524007000029087, 0.0015200789785012603, 0.005374290514737368, -0.00506692985072732, 0.002508002333343029,
                       -0.0003224409883841872, -0.0020520018879324198};


void OQPSKTranscoder::modulate(uint8_t *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal) {
    uint32_t nsym = signal_length / 2;

    // In-phase and quadrature are calculated separately in order to only use a single buffer

    // Calculate in-phase first
    for (uint32_t i = 0; i < signal_length; i += 2) {
        float bit_pos_i = 0;

        switch (2 * signal[i] + signal[i + 1]) {
            case 0:
                bit_pos_i = sqrt(2) / 2;
                break;
            case 1:
                bit_pos_i = -sqrt(2) / 2;
                break;
            case 2:
                bit_pos_i = sqrt(2) / 2;
                break;
            case 3:
                bit_pos_i = -sqrt(2) / 2;
                break;
        }

        for (uint32_t j = 0; j < samples_per_symbol; j++) {
            internal_buffer[samples_per_symbol * i / 2 + j] = bit_pos_i;
            // quadrature_signal[samples_per_symbol/2 + samples_per_symbol*i/2 + j] = bit_pos_q;
        }
    }

    // Set the delay
    for (uint32_t m = 0; m < samples_per_symbol / 2; m++) {
        internal_buffer[samples_per_symbol * nsym + m] = 0;
    }

    filter_fir(oqpsk_taps, NUM_OQPSK_TAPS, internal_buffer, samples_per_symbol * nsym + samples_per_symbol / 2, in_phase_signal);

    // Calculate quadrature
    for (uint32_t i = 0; i < signal_length; i += 2) {
        float bit_pos_q = 0;

        switch (2 * signal[i] + signal[i + 1]) {
            case 0:
                bit_pos_q = sqrt(2) / 2;
                break;
            case 1:
                bit_pos_q = sqrt(2) / 2;
                break;
            case 2:
                bit_pos_q = -sqrt(2) / 2;
                break;
            case 3:
                bit_pos_q = -sqrt(2) / 2;
                break;
        }

        for (uint32_t j = 0; j < samples_per_symbol; j++) {
            internal_buffer[samples_per_symbol / 2 + samples_per_symbol * i / 2 + j] = bit_pos_q;
        }
    }

    // Set the delay
    for (uint32_t m = 0; m < samples_per_symbol / 2; m++) {
        internal_buffer[m] = 0;
    }

    filter_fir(oqpsk_taps, NUM_OQPSK_TAPS, internal_buffer, samples_per_symbol * nsym + samples_per_symbol / 2, quadrature_signal);
}
