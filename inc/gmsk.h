#ifndef TC_SYNCHRONIZATION_GMSK_H
#define TC_SYNCHRONIZATION_GMSK_H
#pragma once

#include <fm.h>
#include <cmath>
#include <iostream>

struct PLLParameters{
    double damping_ratio;
    double natural_frequency;
    double symbol_time;
    double G1; // Gain of the upper branch of the loop filter
    double G2; // Gain of the lower branch of the loop filter
};

class GMSKTranscoder {
private:
    double internal_buffer[1 << 12]; // TODO: Determine size (signal_length * max_samples_per_symbol)
    double internal_buffer2[1 << 12];
    uint32_t sampling_frequency;
    uint32_t sampling_rate;
    uint32_t samples_per_symbol;
    uint32_t max_frequency;
    uint32_t symbol_rate;
    uint32_t max_deviation;
    FMTranscoder fm_transcoder;
    PLLParameters pll_params;

public:
    GMSKTranscoder(uint32_t sample_frequency, uint32_t symb_rate) :
            fm_transcoder(FMTranscoder(sample_frequency, symb_rate / 2, 0,
                                       symb_rate / 4, 0, 0)),
                                       symbol_rate(symb_rate), sampling_frequency(sample_frequency) {
        samples_per_symbol = sample_frequency / symbol_rate;
        max_frequency = symbol_rate / 2;
        max_deviation = symbol_rate / 4;

        double zeta = 0.707;
        double wn = (2.0*10*M_PI*symbol_rate)/4800;
        double an = exp(-zeta*wn/sampling_frequency);
        double g1 = 1 - an*an;
        double g2 = 1 + an*an - 2*an* cos(wn*sqrt(1-zeta*zeta)/sampling_frequency);

        pll_params = {zeta,
                                    wn,
                                    1.0/sampling_frequency,
                                    g1*M_PI/2,
                                    g2*M_PI/2
                                    };
    }

    // TODO: signal_length should be a pre-determined number
    void modulate(int8_t *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal);

    // Consider changing input IQ signal to const
    void
    demodulate(double *input_in_phase_signal, double *input_quadrature_signal, uint16_t signal_length, bool *signal);
};

#endif //TC_SYNCHRONIZATION_GMSK_H
