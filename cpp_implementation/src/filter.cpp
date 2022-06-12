#include "filter.h"

void filter_fir(const double *filter_taps, uint16_t number_of_taps, const double *input_signal, uint16_t size,
                double *output_signal) {
    // TODO: Naive implementation for testing purposes only! If this is mean to be run also on ground, replace with a
    //  well-known library or at least a more efficient, vectorised implementation. Otherwise, fall back to
    //  architecture-specific libraries. Additionally, if the latter is the case, an additional parameter needs to be
    //  passed both here and in the initialization of the modems that defines which implementation is to be chosen
    for (uint16_t n = 0; n < size; n++) {
        output_signal[n] = 0;
        for (uint16_t k = 0; k < fmin(n + 1, number_of_taps); k++) {
            output_signal[n] += filter_taps[k] * input_signal[n - k];
        }
    }
}

// TODO: Instead of O(nk) this can easily be simplified to O(n) by considering the sliding window and at the time step
//  t adding x(t) while subtracting x(t-k). See if there is a HAL implementation for this (ideally don't use convolve).
void integrate(const double *input_signal, uint16_t size, uint16_t number_of_taps, double *output_signal){
    for (uint16_t n = 0; n < size; n++) {
        output_signal[n] = 0;
        for (uint16_t k = 0; k < fmin(n + 1, number_of_taps); k++) {
            output_signal[n] += input_signal[n - k];
        }
    }
}

void multiply_vec(double *signal, uint16_t length, double coef) {
    // TODO: This is a placeholder bad implementation. Replace with HAL function (interface may need to change slightly)
    for (int i = 0; i < length; i++) {
        signal[i] *= coef;
    }
}