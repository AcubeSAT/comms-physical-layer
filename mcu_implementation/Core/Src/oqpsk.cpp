#include "oqpsk.hpp"

void
OQPSKTranscoder::modulate(uint8_t *signal, uint16_t signal_length, float32_t *in_phase_signal, float32_t *quadrature_signal) {
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
   arm_fir_f32(&armFIRInstanceF32, internal_buffer, in_phase_signal, blockLen);
    //filter_fir(oqpsk_taps, 23, internal_buffer, samples_per_symbol * nsym + samples_per_symbol / 2, in_phase_signal);


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

   arm_fir_f32(&armFIRInstanceF32, internal_buffer, quadrature_signal, blockLen);


}