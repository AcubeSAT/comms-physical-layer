#include "fm.h"
#include <algorithm>
#include <math.h>
#include "filter.h"

void FMTranscoder::modulate(const double *signal, uint16_t signal_length, double *in_phase_signal,
                            double *quadrature_signal) {
    // Check if pre-emphasis is applied to the signal
    double central_component = 2 * M_PI * basebandFrequency / samplingFrequency;
    double frequency_deviation_component = 2 * M_PI * maxDeviation / samplingFrequency;
    if (predeEmphasis) {
        filter_fir(pre_emphasis_filter, 2, signal, signal_length, in_phase_signal);
        // TODO: Replace this with HAL function

        // Normalization
        double max_el = *(std::max_element(in_phase_signal, in_phase_signal + signal_length));
        multiply_vec(in_phase_signal, signal_length, 1.0 / max_el);
    }

    double phase = 0;
    double step;
    double tx_curr = 0;
    double tx_next = 0;
    for (uint16_t i = 0; i < signal_length; i++) {
        // TODO: More efficient way
        if (predeEmphasis) {
            step = central_component + frequency_deviation_component * in_phase_signal[i];
        } else {
            step = central_component + frequency_deviation_component * signal[i];
        }
        phase += step;
        phase -= floor(phase / (2 * M_PI)) * 2 * M_PI;
        in_phase_signal[i] = cos(phase);
        quadrature_signal[i] = sin(phase);
    }


}