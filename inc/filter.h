#ifndef TC_SYNCHRONIZATION_FILTER_H
#define TC_SYNCHRONIZATION_FILTER_H
#pragma once

#include <math.h>
#include <cstdint>

// Filtering is supposed to be architecture-specific. When porting to STM32 make sure to make use of the existing HAL
// library functions (etc `LL_FMAC_FUNC_CONVO_FIR`)

void filter_fir(const double *filter_taps, uint16_t number_of_taps, const double *input_signal, uint16_t size,
                double *output_signal);

void multiply_vec(double *signal, uint16_t length, double coef);

#endif //TC_SYNCHRONIZATION_FILTER_H
