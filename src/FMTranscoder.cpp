#include "FMTranscoder.hpp"
#include "filter.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

void FMTranscoder::modulate(const double *signal, uint16_t signal_length, double *in_phase_signal,
                            double *quadrature_signal) {

    // Central component should be 0 for baseband
    double central_component = 2 * M_PI * basebandFrequency / samplingFrequency;
    double frequency_deviation_component = 2 * M_PI * maxDeviation / samplingFrequency;

    // Check if pre-emphasis is applied to the signal
    if (predeEmphasis) {
        // Attenuate higher frequencies
        filterFIR(preEmphasisFilter, 2, signal, signal_length, in_phase_signal);
        // TODO: Replace this with HAL function

        // Normalization
        double max_el = *(std::max_element(in_phase_signal, in_phase_signal + signal_length));
        multiplyVector(in_phase_signal, signal_length, 1.0 / max_el);
    }

    double phase = 0;
    double step;
    for (uint16_t i = 0; i < signal_length; i++) {
        // TODO: More efficient way
        // The conditional is here because in the case of pre-emphasis, in_phase_signal is used as a temporary
        // buffer for the filtered signal
        if (predeEmphasis) {
            step = central_component + frequency_deviation_component * in_phase_signal[i];
        } else {
            step = central_component + frequency_deviation_component * signal[i];
        }
        // phase adjustment
        phase += step;
        phase -= floor(phase / (2 * M_PI)) * 2 * M_PI;
        // For being more efficient, this probably
        quadrature_signal[i] = sin(phase);
        in_phase_signal[i] = cos(phase);
    }
}
