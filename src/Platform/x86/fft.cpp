#include <cmath>
#include <complex>
#include <iostream>
#include <valarray>

#include "fft.hpp"

constexpr double PI = 3.141592653589793238460;

void fft(ComplexArray& x)
{
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    ComplexArray even(N / 2);
    ComplexArray odd(N / 2);

    for (int i = 0; i < N / 2; ++i) {
        even[i] = x[2 * i];
        odd[i] = x[2 * i + 1];
    }

    // conquer
    fft(even);
    fft(odd);

    // combine
    for (size_t k = 0; k < N/2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}

void fftshift(ComplexArray& complexVector, const size_t count) {
    int center = static_cast<int>(std::floor(static_cast<float>(count/2)));
    if (count % 2 != 0) {
        center++;
    }

    std::rotate(complexVector.begin(),complexVector.begin() + center,complexVector.begin() + count);
}
