#pragma once

#include "GMSKTranscoder.hpp"
#include "fft.hpp"

constexpr uint8_t asmLength = 64;
constexpr uint8_t tailLength = 64;
constexpr uint16_t MaxPowerOf2 = 256;

template <int samplesPerSymbol, int inputLength> class Synchronizer {
private:
    /**
     * In phase component of the GMSK modulated ASM
     */
    double inPhaseASMSamples[asmLength * samplesPerSymbol] = {};

    /**
     * Quadrature component of the GMSK modulated ASM
     */
    double quadratureASMSamples[asmLength * samplesPerSymbol] = {};

    /**
     * Acquisition Buffer that contains the output of the correlation algorithm
     */
    double acqBuffer[(samplesPerSymbol * (inputLength - asmLength)) / (samplesPerSymbol / 4)][MaxPowerOf2] = {};

public:
    /**
     * ASM test sequence to be detected by the algorithm
     */
    const etl::bitset<asmLength> startSequence = 0b1110110101001000100011101001110110110110010010000001001110110011;

    /**
     * Tail sequence of a CLTU
     * @see CCSDS 230.1-G-3 5.5.1
     */
    constexpr static bool tailSequence[tailLength] = {1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1,
                              1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1,
                              1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1,
                              1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1};

    uint16_t sampleFrequency;
    uint16_t symbolRate;
    GMSKTranscoder<samplesPerSymbol, asmLength> gmsk;

    /**
     * Calculates the I/Q of the GMSK modulated ASM
     */
    Synchronizer(int sampleFrequency, int symbolRate) : sampleFrequency(sampleFrequency), symbolRate(symbolRate),
            gmsk(GMSKTranscoder<samplesPerSymbol, asmLength>(sampleFrequency, symbolRate, false)){

        gmsk.modulate(startSequence, inPhaseASMSamples, quadratureASMSamples);
    }

    /**
     * Computes the correlation between the CLTU and the ASM header
     *
     * @param inPhaseSignal : In phase component of GMSK modulated CLTU
     * @param quadratureSignal : Quadrature component of GMSK modulated CLTU
     * @param signalLength : Number of samples
     */
    void computeCorrelation(double *inPhaseSignal, double *quadratureSignal);

    double getAcqBuffer(uint16_t i, uint16_t j){
        return acqBuffer[i][j];
    };
};


