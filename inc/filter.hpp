#pragma once

#include <cmath>
#include <cstdint>

constexpr uint8_t inPhaseSync = 0b10;
constexpr uint8_t quadratureSync = 0b01;

// Filtering is supposed to be architecture-specific. When porting to STM32 make
// sure to make use of the existing HAL library functions (etc
// `LL_FMAC_FUNC_CONVO_FIR`)

void filterFIR(const double *filterTaps, uint16_t numberOfTaps,
               const double *inputSignal, uint16_t size, double *outputSignal);

void integrate(const double *inputSignal, uint16_t size, uint16_t numberOfTaps,
               double *outputSignal);

void multiplyVector(double *signal, uint16_t length, double coef);

void packetizeIQ(double *inPhaseSignal, double *quadratureSignal,
                 uint16_t length, uint32_t *outputSignal);