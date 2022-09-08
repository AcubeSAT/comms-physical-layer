#pragma once

#include <complex>
#include <iostream>
#include "etl/vector.h"

constexpr uint16_t MaxFFTVectorLength = 10000;

typedef std::complex<double> Complex;
typedef etl::vector<Complex, MaxFFTVectorLength> ComplexArray;

/**
 *  Cooley–Tukey FFT algorithm
 *  (https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B)
 *
 *  @param x : input signal of the FFT
 */
void fft(ComplexArray& x);

/**
 * Shifts the output of the FFT to put the samples in the correct order
 *
 * @param complexVector : input complex signal (output of the FFT)
 * @param count : input signal's size
 */
void fftshift(ComplexArray& complexVector, size_t count);


