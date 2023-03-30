#pragma once

#include <FMTranscoder.hpp>
#include "GMSKFilterTaps.hpp"
#include <cmath>
#include "etl/bitset.h"
#include <iostream>

struct PLLParameters{
    float dampingRatio;
    float naturalFrequency;
    float symbolTime;
    float G1; // Gain of the upper branch of the loop filter
    float G2; // Gain of the lower branch of the loop filter
};


template <uint8_t samplesPerSymbol, uint16_t inputLength> class GMSKTranscoder {
private:
    float internalBufferInPhase[samplesPerSymbol * inputLength];
    float internalBufferQuadrature[samplesPerSymbol * inputLength];
    static constexpr uint8_t wienerTapNumber = 3;
    static constexpr uint8_t wienerSymbolDelay = 2;

    float wienerTaps[wienerTapNumber] = {-0.0859984, 1.0116342, -0.0859984};

    float delayedTaps[wienerTapNumber * wienerSymbolDelay * samplesPerSymbol];
    bool equalize;
    float convolvedFilters[109]; //TODO: Length is the length of gmsk_mod and delayed taps added (Now 60 + 49)
    uint32_t samplingFrequency;
    uint32_t samplingRate;
    uint32_t maxFrequency;
    uint32_t symbolRate;
    uint32_t maxDeviation;
    FMTranscoder fmTranscoder;
    PLLParameters pllParams;
    const float *gmskModulationCoefficients = nullptr;

    /**
     * Enumeration of samples per symbol values
     */
    enum SPS {SPS_4 = 4, SPS_6 = 6, SPS_10 = 10};

public:
    GMSKTranscoder(uint32_t sampleFrequency, uint32_t symbRate, bool equalize) :
            fmTranscoder(FMTranscoder(sampleFrequency, symbRate / 2, 0,
                                      symbRate / 4, 0, 0)),
            symbolRate(symbRate), samplingFrequency(sampleFrequency), equalize(equalize) {

        maxFrequency = symbolRate / 2;
        maxDeviation = symbolRate / 4;

        // Adding 2-symbol delay between taps
        std::fill(std::begin(delayedTaps), std::begin(delayedTaps) + 2*samplesPerSymbol, wienerTaps[0]);
        std::fill(std::begin(delayedTaps) + 2*samplesPerSymbol, std::begin(delayedTaps) + 4*samplesPerSymbol, wienerTaps[1]);
        std::fill(std::begin(delayedTaps) + 4*samplesPerSymbol, std::begin(delayedTaps) + 6*samplesPerSymbol, wienerTaps[2]);

        float zeta = 0.707;
        float wn = (2.0*10*M_PI*symbolRate)/4800;
        float an = exp(-zeta*wn/samplingFrequency);
        float g1 = 1 - an*an;
        float g2 = 1 + an*an - 2*an* cos(wn*sqrt(1-zeta*zeta)/samplingFrequency);

        pllParams = {zeta,
                                    wn,
                                    static_cast<float>(1.0)/samplingFrequency,
                                    static_cast<float>(g1*M_PI/2),
                                    static_cast<float>(g2*M_PI/2)
                                    };

        switch (samplesPerSymbol) {
            case SPS_4:
                gmskModulationCoefficients = GMSKSPS4BTs25;
                break;
            case SPS_6:
                gmskModulationCoefficients = GMSKSPS6BTs25;
                break;
            case SPS_10:
                gmskModulationCoefficients = GMSKSPS10BTs25;
                break;
            default:
                std::cerr << "Value Error: samplesPerSymbol value is not supported\n";
        }
    }

    // TODO: signal_length should be a pre-determined number
    void modulate(const etl::bitset<inputLength>& input, float *inPhaseSignal, float *quadratureSignal);

    // Consider changing input IQ signal to const
    void
    demodulate(float *inputInPhaseSignal, float *inputQuadratureSignal, uint16_t signalLength, etl::bitset<inputLength>& output);

};
