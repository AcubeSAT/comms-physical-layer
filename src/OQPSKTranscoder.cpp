#include "filter.hpp"
#include <OQPSKTranscoder.hpp>
#include <iostream>

#define NUM_OQPSK_TAPS 23

double oqpskTaps[] = {-0.0020520018879324198, -0.0003224409883841872, 0.002508002333343029, -0.00506692985072732, 0.005374290514737368,
                       0.0015200789785012603, -0.007524007000029087, 0.021281106397509575, -0.03762003779411316, -0.05320276319980621, 0.2901403307914734,
                       0.5699287056922913, 0.2901403307914734, -0.05320276319980621, -0.03762003779411316, 0.021281106397509575,
                       -0.007524007000029087, 0.0015200789785012603, 0.005374290514737368, -0.00506692985072732, 0.002508002333343029,
                       -0.0003224409883841872, -0.0020520018879324198};


void OQPSKTranscoder::modulate(bool *signal, uint16_t signalLength, double *inPhaseSignal, double *quadratureSignal) {
    uint32_t nsym = signalLength / 2;

    // In-phase and quadrature are calculated separately in order to only use a single buffer

    // Calculate in-phase first
    for (uint32_t i = 0; i < signalLength; i += 2) {
        float bitPosI = 0;

        switch (2 * signal[i] + signal[i + 1]) {
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
            // quadrature_signal[samplesPerSymbol/2 + samplesPerSymbol*i/2 + j] = bit_pos_q;
        }
    }

    // Set the delay
    for (uint32_t m = 0; m < samplesPerSymbol / 2; m++) {
        internalBuffer[samplesPerSymbol * nsym + m] = 0;
    }

    filterFIR(oqpskTaps, NUM_OQPSK_TAPS, internalBuffer, samplesPerSymbol * nsym + samplesPerSymbol / 2,
              inPhaseSignal);

    // Calculate quadrature
    for (uint32_t i = 0; i < signalLength; i += 2) {
        float bit_pos_q = 0;

        switch (2 * signal[i] + signal[i + 1]) {
            case 0:
                bit_pos_q = sqrt(2) / 2;
                break;
            case 1:
                bit_pos_q = sqrt(2) / 2;
                break;
            case 2:
                bit_pos_q = -sqrt(2) / 2;
                break;
            case 3:
                bit_pos_q = -sqrt(2) / 2;
                break;
        }

        for (uint32_t j = 0; j < samplesPerSymbol; j++) {
            internalBuffer[samplesPerSymbol / 2 + samplesPerSymbol * i / 2 + j] = bit_pos_q;
        }
    }

    // Set the delay
    for (uint32_t m = 0; m < samplesPerSymbol / 2; m++) {
        internalBuffer[m] = 0;
    }

    filterFIR(oqpskTaps, NUM_OQPSK_TAPS, internalBuffer, samplesPerSymbol * nsym + samplesPerSymbol / 2,
              quadratureSignal);
}
