#pragma once

#include <math.h>
#include <cstdint>

// Filtering is supposed to be architecture-specific. When porting to STM32 make sure to make use of the existing HAL
// library functions (etc `LL_FMAC_FUNC_CONVO_FIR`)

void filter_fir(const double *filter_taps, uint16_t number_of_taps, const double *input_signal, uint16_t size,
                double *output_signal);

void integrate(const double *input_signal, uint16_t size, uint16_t number_of_taps, double *output_signal);

void multiply_vec(double *signal, uint16_t length, double coef);

inline bool signs(double x){
    return x > 0;
}