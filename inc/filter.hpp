#pragma once

#include <cmath>
#include <cstdint>

constexpr uint8_t inPhaseSync = 0b10;
constexpr uint8_t quadratureSync = 0b01;

// Filtering is supposed to be architecture-specific. When porting to STM32 make
// sure to make use of the existing HAL library functions (etc
// `LL_FMAC_FUNC_CONVO_FIR`)

void filterFIR(const float *filterTaps, uint16_t numberOfTaps,
               const float *inputSignal, uint16_t size, float *outputSignal);

void integrate(const float *inputSignal, uint16_t size, uint16_t numberOfTaps,
               float *outputSignal);

void multiplyVector(float *signal, uint16_t length, float coef);

void packetizeIQ(double *inPhaseSignal, double *quadratureSignal,
                 uint16_t length, uint32_t *outputSignal);
