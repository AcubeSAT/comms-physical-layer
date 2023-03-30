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
    float preEmphasisFilter[2];

public:
    /**
     *
     * @param maxModulatingFrequency  Highest frequency component present in the modulated signal
     * @param maxDeviation     Maximum deviation from the carrier frequency
     * @param preDeEmphasis   Defines whether pre- and de-emphasis filters of high frequencies is applied
     */
    FMTranscoder(uint32_t samplingFrequency, uint32_t maxModulatingFrequency, uint32_t basebandFrequency,
                 uint16_t maxDeviation, bool preDeEmphasis, bool limitDeltaPhase) : samplingFrequency(
            samplingFrequency),
                                                                                         maxModulatingFrequency(
                                                                                                 maxModulatingFrequency),
                                                                                         maxDeviation(maxDeviation),
                                                                                         predeEmphasis(preDeEmphasis),
                                                                                         basebandFrequency(
                                                                                                 basebandFrequency),
                                                                                         limitDeltaPhase(
                                                                                                 limitDeltaPhase) {
        // Estimate bandwidth using Carlson's rule
        bandwidth = 2 * (maxModulatingFrequency + maxDeviation);

        // Initialize pre-emphasis filter taps
        preEmphasisFilter[0] = 1.0;
        const float timeConstCutoff = 50e-6; // τ = 50µs in Europe
        preEmphasisFilter[1] = -(1.0 - 1.0 / (timeConstCutoff * samplingFrequency));
        centralFrequency = (2.0 * maxModulatingFrequency) / samplingFrequency;
    };

    void modulate(const float *signal, uint16_t signal_length, float *in_phase_signal, float *quadrature_signal);
};