#ifndef TC_SYNCHRONIZATION_OQPSK_H
#define TC_SYNCHRONIZATION_OQPSK_H

#include <cstdint>

class OQPSKTranscoder {

private:
    uint32_t sampling_frequency;
    uint32_t sampling_rate;
    uint32_t samples;
    uint8_t bits_per_sample = 2;
    uint32_t samples_per_symbol;
    double internal_buffer[1 << 12]; // TODO: Determine size (signal_length * max_samples_per_symbol)

public:
    OQPSKTranscoder(uint32_t sample_rate):sampling_frequency(4*sample_rate), sampling_rate(sample_rate){
        samples_per_symbol = sampling_frequency/sampling_rate; // samples_per_symbol and samples_per_symbol/2 must be integer values - throw error?
    }

    void modulate(uint8_t *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal);

};
#endif //TC_SYNCHRONIZATION_OQPSK_H
