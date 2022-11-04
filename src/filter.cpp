#include "filter.hpp"

void filterFIR(const double *filterTaps, uint16_t numberOfTaps,
               const double *inputSignal, uint16_t size, double *outputSignal) {
    // TODO: Naive implementation for testing purposes only! If this is mean to
    // be run also on ground, replace with a
    //  well-known library or at least a more efficient, vectorised
    //  implementation. Otherwise, fall back to architecture-specific libraries.
    //  Additionally, if the latter is the case, an additional parameter needs
    //  to be passed both here and in the initialization of the modems that
    //  defines which implementation is to be chosen
    for (uint16_t n = 0; n < size; n++) {
        outputSignal[n] = 0;
        for (uint16_t k = 0; k < fmin(n + 1, numberOfTaps); k++) {
            outputSignal[n] += filterTaps[k] * inputSignal[n - k];
        }
    }
}

// TODO: Instead of O(nk) this can easily be simplified to O(n) by considering
// the sliding window and at the time step
//  t adding x(t) while subtracting x(t-k). See if there is a HAL implementation
//  for this (ideally don't use convolve).
void integrate(const double *inputSignal, uint16_t size, uint16_t numberOfTaps,
               double *outputSignal) {
    for (uint16_t n = 0; n < size; n++) {
        outputSignal[n] = 0;
        for (uint16_t k = 0; k < fmin(n + 1, numberOfTaps); k++) {
            outputSignal[n] += inputSignal[n - k];
        }
    }
}

void multiplyVector(double *signal, uint16_t length, double coef) {
    // TODO: This is a placeholder bad implementation. Replace with HAL function
    // (interface may need to change slightly)
    for (int i = 0; i < length; i++) {
        signal[i] *= coef;
    }
}

/**
 * Accepts an in-phase and quadrature input stream and converts it to a packet
 *stream for feeding into the AT86RF215 I/Q interface 0b111100000000
 * @param inPhaseSignal     In-phase signal
 * @param quadratureSignal  Quadrature signal
 * @param length            Length of the signal (quadrature and in-phase)
 * @param outputSignal      Output buffer
 */
void packetizeIQ(double *inPhaseSignal, double *quadratureSignal,
                 uint16_t length, uint32_t *outputSignal) {
    for (uint16_t i = 0; i < length; i++) {

        // Calculate I component
        uint16_t inphase =
            (inPhaseSignal[i] >= 0)
                ? static_cast<uint16_t>(inPhaseSignal[i] * 0x1FFF)
                : ((static_cast<uint16_t>(std::abs(inPhaseSignal[i] * 0x1FFF)) ^ 0x3FFF) + 1) & 0x3FFF;

        // Calculate Q component
        uint16_t quadrature =
            (quadratureSignal[i] >= 0)
                ? static_cast<uint16_t>(quadratureSignal[i] * 0x1FFF)
                : ((static_cast<uint16_t>(std::abs(quadratureSignal[i] * 0x1FFF)) ^ 0x3FFF) + 1) & 0x3FFF;

        outputSignal[i] =
            (inPhaseSync << 30U) | static_cast<uint32_t>(inphase) << 16U |
            (quadratureSync << 14U) | static_cast<uint32_t>(quadrature);
    }
}