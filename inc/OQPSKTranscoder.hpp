#pragma once

#include "GMSKTranscoder.hpp"
#include <FMTranscoder.hpp>
#include <cmath>
#include <cstdint>

class OQPSKTranscoder {

private:
    double internal_buffer_in_phase[60000]; // TODO: Determine size (signal_length * max_samples_per_symbol)
    double internal_buffer_quadrature[60000];
    uint32_t sampling_frequency;
    uint32_t symbol_rate;
    uint32_t samples;
    uint8_t bits_per_sample = 2;
    uint32_t samples_per_symbol;
    uint32_t max_frequency;
    uint32_t max_deviation;
    FMTranscoder fm_transcoder;
    double internal_buffer[1 << 12]; // TODO: Determine size (signal_length * max_samples_per_symbol)

public:
    OQPSKTranscoder(uint32_t sampling_frequency, uint32_t symbol_rate):sampling_frequency(sampling_frequency), symbol_rate(symbol_rate),
                                          fm_transcoder(FMTranscoder(sampling_frequency, symbol_rate / 2, 0,
                                                                     symbol_rate / 4, 0, 0)) {
        samples_per_symbol = sampling_frequency/symbol_rate; // samples_per_symbol and samples_per_symbol/2 must be integer values - throw error?
        max_frequency = symbol_rate / 2;
        max_deviation = symbol_rate / 4;
    }

    void modulate(uint8_t *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal);

};