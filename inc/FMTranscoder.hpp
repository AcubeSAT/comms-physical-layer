#pragma once

#include <cstdint>

class FMTranscoder {
private:
    uint32_t samplingFrequency;
    uint32_t maxModulatingFrequency;
    uint32_t centralFrequency;
    uint16_t maxDeviation;
    uint32_t bandwidth;
    uint32_t basebandFrequency;
    bool predeEmphasis;
    bool limitDeltaPhase;
    double pre_emphasis_filter[2];

public:
    /**
     *
     * @param max_modulating_frequency  Highest frequency component present in the modulated signal
     * @param max_deviation     Maximum deviation from the carrier frequency
     * @param pre_de_emphasis   Defines whether pre- and de-emphasis filters of high frequencies is applied
     */
    FMTranscoder(uint32_t sampling_frequency, uint32_t max_modulating_frequency, uint32_t baseband_frequency,
                 uint16_t max_deviation, bool pre_de_emphasis, bool limit_delta_phase) : samplingFrequency(
            sampling_frequency),
                                                                                         maxModulatingFrequency(
                                                                                                 max_modulating_frequency),
                                                                                         maxDeviation(max_deviation),
                                                                                         predeEmphasis(pre_de_emphasis),
                                                                                         basebandFrequency(
                                                                                                 baseband_frequency),
                                                                                         limitDeltaPhase(
                                                                                                 limit_delta_phase) {
        // Estimate bandwidth using Carlson's rule
        bandwidth = 2 * (maxModulatingFrequency + maxDeviation);

        // Initialize pre-emphasis filter taps
        pre_emphasis_filter[0] = 1.0;
        const double time_const_cutoff = 50e-6; // τ = 50µs in Europe
        pre_emphasis_filter[1] = -(1.0 - 1.0 / (time_const_cutoff * sampling_frequency));
        centralFrequency = (2.0 * maxModulatingFrequency) / samplingFrequency;
    };

    void modulate(const double *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal);
};