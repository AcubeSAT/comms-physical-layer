#include "Synchronizer.hpp"


template <int samplesPerSymbol, int inputLength>
void Synchronizer<samplesPerSymbol, inputLength>::computeCorrelation(float *inPhaseSignal, float *quadratureSignal) {

    const uint16_t step = samplesPerSymbol / 4;

    for (int i = 0; i < (inputLength - asmLength) * samplesPerSymbol / step; ++i) {
        int offset = i * step;

        ComplexArray multipliedSignals(MaxPowerOf2);
        multipliedSignals.fill(0);

        // Multiplication with GMSK modulated ASM's conjugate
        for (int j = 0; j < asmLength * samplesPerSymbol; ++j) {
            Complex asmSamples(inPhaseASMSamples[j], quadratureASMSamples[j]);
            Complex inputSamples(inPhaseSignal[offset + j], quadratureSignal[offset + j]);

            multipliedSignals[j] = conj(asmSamples) * inputSamples;
        }

        fft(multipliedSignals);

        // Perform the FFT Shift operation
        fftshift(multipliedSignals, MaxPowerOf2);

        // Calculate the norm of each sample
        for (int j = 0; j < MaxPowerOf2; ++j) {
            acqBuffer[i][j] = std::abs(multipliedSignals[j]) * std::abs(multipliedSignals[j]);
        }
    }
}

template class Synchronizer<10, 64>;
template class Synchronizer<10, 176>;
template class Synchronizer<10, 160>;