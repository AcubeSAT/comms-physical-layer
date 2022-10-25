#include "filter.hpp"
#include <OQPSKTranscoder.hpp>


template <uint16_t inputLength>
void OQPSKTranscoder<inputLength>::modulate(const etl::bitset<inputLength>& input, double *inPhaseSignal, double *quadratureSignal) {
    // In-phase and quadrature are calculated separately in order to only use a single buffer
    // Calculate in-phase first
    for (uint32_t i = 0; i < inputLength; i += 2) {
        float bitPosI = 0;

        switch (2 * input[i] + input[i + 1]) {
        case 0:
            bitPosI = sqrt(2) / 2;
            break;
        case 1:
            bitPosI = -sqrt(2) / 2;
            break;
        case 2:
            bitPosI = sqrt(2) / 2;
            break;
        case 3:
            bitPosI = -sqrt(2) / 2;
            break;
        }
        for (uint32_t j = 0; j < samplesPerSymbol; j++) {
            internalBuffer[samplesPerSymbol * i / 2 + j] = bitPosI;
        }
    }
    // Set the delay
    for (uint32_t m = 0; m < samplesPerSymbol / 2; m++) {
        internalBuffer[samplesPerSymbol * numberOfSymbols + m] = 0;
    }
    filterFIR(oqpskTaps, numOQPSKTaps, internalBuffer, samplesPerSymbol * numberOfSymbols + samplesPerSymbol / 2,
              inPhaseSignal);

    // Calculate quadrature
    for (uint32_t i = 0; i < inputLength; i += 2) {
        float bitPosQ = 0;

        switch (2 * input[i] + input[i + 1]) {
        case 0:
            bitPosQ = sqrt(2) / 2;
            break;
        case 1:
            bitPosQ = sqrt(2) / 2;
            break;
        case 2:
            bitPosQ = -sqrt(2) / 2;
            break;
        case 3:
            bitPosQ = -sqrt(2) / 2;
            break;
        }
        for (uint32_t j = 0; j < samplesPerSymbol; j++) {
            internalBuffer[samplesPerSymbol / 2 + samplesPerSymbol * i / 2 + j] = bitPosQ;
        }
    }
    // Set the delay
    for (uint32_t m = 0; m < samplesPerSymbol / 2; m++) {
        internalBuffer[m] = 0;
    }
    filterFIR(oqpskTaps, numOQPSKTaps, internalBuffer, samplesPerSymbol * numberOfSymbols + samplesPerSymbol / 2,
              quadratureSignal);
}

// OQPSKTranscoder with input size 40 is used in oqpskTest.cpp
template class OQPSKTranscoder<40>;