#include "filter.hpp"
// extern "C"  is due to some stupid bug with nested externs in Drivers/CMSIS/Include/cmsis_gcc.h
// This is the dummy solution, other solutions may be
// a. updating cmsis header files to >= 5.7 or
// b. moving lines 133 - 149 of the header mentioned above out of the function they are currently in, making them global.
#ifdef STM32
extern "C" {
    #include "arm_math.h"
}
#endif

void filterFIR(const double *filterTaps, uint16_t numberOfTaps,
               const double *inputSignal, uint16_t size, double *outputSignal) {
    //  Additionally, if the latter is the case, an additional parameter needs
    //  to be passed both here and in the initialization of the modems that
    //  defines which implementation is to be chosen
#ifndef STM32
    for (uint16_t n = 0; n < size; n++) {
        outputSignal[n] = 0;
        for (uint16_t k = 0; k < fmin(n + 1, numberOfTaps); k++) {
            outputSignal[n] += filterTaps[k] * inputSignal[n - k];
        }
    }
#else
    arm_fir_instance_f32 firInst;
    // find max size and max numberOfTaps to make firState static
    float firState[size + numberOfTaps - 1];
    arm_fir_init_f32(&firInst, numberOfTaps, (float *)filterTaps, firState, size);
    arm_fir_f32(&firInst, (float *)inputSignal, (float *)outputSignal, size);
#endif
}

// TODO: See if there is a HAL implementation
//  for this (ideally don't use convolve).
void integrate(const double *inputSignal, uint16_t size, uint16_t numberOfTaps,
               double *outputSignal) {
    outputSignal[0] = inputSignal[0]; 
    for (uint16_t n = 1; n < size; n++) {
        outputSignal[n] = outputSignal[n - 1] + inputSignal[n];  
        if (n >= numberOfTaps)
            outputSignal[n] -= inputSignal[n - numberOfTaps]; 
    }
}

void multiplyVector(double *signal, uint16_t length, double coef) {
    // TODO: This is a placeholder bad implementation. Replace with HAL function
    // (interface may need to change slightly)
#ifndef STM32
    for (int i = 0; i < length; i++) {
        signal[i] *= coef;
    }
#else
    arm_scale_f32((float *)signal, coef, (float *)signal, length);
#endif
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
