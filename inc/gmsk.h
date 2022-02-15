#ifndef TC_SYNCHRONIZATION_GMSK_H
#define TC_SYNCHRONIZATION_GMSK_H
#pragma once

#include <fm.h>

class GMSKTranscoder {
private:
    double internal_buffer[1 << 12]; // TODO: Determine size (signal_length * max_samples_per_symbol)
    double internal_buffer2[1 << 12];
    uint32_t sampling_frequency;
    uint32_t sampling_rate;
    uint32_t samples_per_symbol;
    uint32_t max_frequency;
    uint32_t max_deviation;
    FMTranscoder fm_transcoder;

public:
    GMSKTranscoder(uint32_t sampling_frequency, uint32_t symbol_rate) :
            fm_transcoder(FMTranscoder(sampling_frequency, symbol_rate / 2, 0,
                                       symbol_rate / 4, 0, 0)) {
        samples_per_symbol = sampling_frequency / symbol_rate;
        max_frequency = symbol_rate / 2;
        max_deviation = symbol_rate / 4;

    }

    // TODO: signal_length should be a pre-determined number
    void modulate(const double *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal);

    // Consider changing input IQ signal to const
    void
    demodulate(double *input_in_phase_signal, double *input_quadrature_signal, uint16_t signal_length, bool *signal);
};

#endif //TC_SYNCHRONIZATION_GMSK_H
