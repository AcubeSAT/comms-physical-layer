#pragma once

#include <math.h>
#include <cstdint>

// Filtering is supposed to be architecture-specific. When porting to STM32 make sure to make use of the existing HAL
// library functions (etc `LL_FMAC_FUNC_CONVO_FIR`)

void filterFIR(const double *filterTaps, uint16_t numberOfTaps, const double *inputSignal, uint16_t size,
               double *outputSignal);

void integrate(const double *inputSignal, uint16_t size, uint16_t numberOfTaps, double *outputSignal);

void multiplyVector(double *signal, uint16_t length, double coef);

inline bool signs(double x){
    return x > 0;
}