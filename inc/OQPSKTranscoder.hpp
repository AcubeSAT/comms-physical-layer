#pragma once

#include "GMSKTranscoder.hpp"
#include <FMTranscoder.hpp>
#include <cmath>
#include <cstdint>

class OQPSKTranscoder {

private:
    double internalBufferInPhase[60000]; // TODO: Determine size (signal_length * maxSamplesPerSymbol)
    double internalBufferQuadrature[60000];
    uint32_t samplingFrequency;
    uint32_t symbolRate;
    uint32_t samples;
    uint8_t bitsPerSample = 2;
    uint32_t samplesPerSymbol;
    uint32_t maxFrequency;
    uint32_t maxDeviation;
    FMTranscoder fmTranscoder;
    double internalBuffer[1 << 12]; // TODO: Determine size (signal_length * maxSamplesPerSymbol)

public:
    OQPSKTranscoder(uint32_t samplingFrequency, uint32_t symbolRate):samplingFrequency(samplingFrequency), symbolRate(symbolRate),
                                          fmTranscoder(FMTranscoder(samplingFrequency, symbolRate / 2, 0,
                                                                     symbolRate / 4, 0, 0)) {
        samplesPerSymbol = samplingFrequency/symbolRate; // samplesPerSymbol and samplesPerSymbol/2 must be integer values - throw error?
        maxFrequency = symbolRate / 2;
        maxDeviation = symbolRate / 4;
    }

    void modulate(bool *signal, uint16_t signalLength, double *inPhaseSignal, double *quadratureSignal);

};