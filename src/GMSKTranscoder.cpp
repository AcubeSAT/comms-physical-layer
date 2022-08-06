#include "filter.hpp"
#include <GMSKTranscoder.hpp>
#include <cstring>
#include <iostream>

template <int samplesPerSymbol>
void GMSKTranscoder<samplesPerSymbol>::modulate(bool *signal, uint16_t signalLength, double *inPhaseSignal,
                              double *quadratureSignal) {

    uint16_t samplesN = samplesPerSymbol * signalLength;

    internalBufferQuadrature[0] = (signal[0]+1)%2;
    for (uint16_t i = 0; i < signalLength - 1; i++){
        internalBufferQuadrature[i+1] = (signal[i+1]+signal[i] + i+1)%2;
    }

    // NRZ encoding
    for (uint16_t i = 0; i < signalLength; i++) {
        // TODO: HAL implementation
        for (uint16_t j = 0; j < samplesPerSymbol; j++) {
            internalBufferInPhase[i * samplesPerSymbol + j] = -1 + 2 * (internalBufferQuadrature[i]);
        }
    }

    // Precoding
    uint16_t s = 41;

    // Filter matching
    filterFIR(gmskModulationCoefficients, s, internalBufferInPhase, samplesN, internalBufferQuadrature);

    // FM Modulator
    fmTranscoder.modulate(internalBufferQuadrature, samplesN, inPhaseSignal, quadratureSignal);
}

template <int samplesPerSymbol>
void GMSKTranscoder<samplesPerSymbol>::demodulate(double *inputInPhaseSignal, double *inputQuadratureSignal, uint16_t signalLength,
                                bool *signal) {

    uint16_t symbolsN = floor(signalLength / samplesPerSymbol);
    float frequency_deviation_component = 2 * M_PI * maxDeviation / samplingFrequency;
    double rx_int[2 << 12] = {0};
    int numtaps = 41;

    // Add Wiener Filter for equalisation (Convolution with the gaussian filter)
    if(equalize){
        filterFIR(delayedTaps, 6 * samplesPerSymbol, gmskModulationCoefficients, numtaps, convolvedFilters);

        filterFIR(convolvedFilters, numtaps, inputInPhaseSignal, signalLength, internalBufferInPhase);
        filterFIR(convolvedFilters, numtaps, inputQuadratureSignal, signalLength, internalBufferQuadrature);
    }
    else {
        filterFIR(gmskModulationCoefficients, numtaps, inputInPhaseSignal, signalLength, internalBufferInPhase);
        filterFIR(gmskModulationCoefficients, numtaps, inputQuadratureSignal, signalLength, internalBufferQuadrature);
    }

    integrate(internalBufferInPhase, signalLength, 2 * samplesPerSymbol, inputInPhaseSignal);
    integrate(internalBufferQuadrature, signalLength, 2 * samplesPerSymbol, inputQuadratureSignal);

    // Implement PLL
    double dco = 0;
    double lower = 0;
    double loopFilt = 0;
    double phaseError = 0;
    double timingClockPhase = 0;
    uint16_t timingWindow = 10*samplesPerSymbol; // TODO: 200*samplesPerSymbol;
    double timingAngle = 0;
    double* timingAngleLog = internalBufferInPhase;
    double* toff = internalBufferQuadrature;

    double temp;

    for (uint16_t i; i < signalLength; i++){
        if (i > 1 && (i+1) % timingWindow == 0){
            double iClockPhase = 0;
            double qClockPhase = 0;
            for (uint16_t j = i - timingWindow + 1; j <= i; j++){
                iClockPhase += fabs(inputInPhaseSignal[j]) * cos((j + 1) * 2 * M_PI * (symbolRate / 2.0) / samplingFrequency);
                qClockPhase -= fabs(inputInPhaseSignal[j]) * sin((j + 1) * 2 * M_PI * (symbolRate / 2.0) / samplingFrequency);
                }
            timingAngle = atan2(qClockPhase, iClockPhase);
            timingClockPhase = timingAngle;
        } else{
            timingClockPhase += 2.0*M_PI/(2*samplesPerSymbol);
        }
        timingAngleLog[i] = timingAngle;

        temp = inputInPhaseSignal[i] * cos(dco) + inputQuadratureSignal[i] * sin(dco);
        inputQuadratureSignal[i] = -inputInPhaseSignal[i] * sin(dco) + cos(dco) * inputQuadratureSignal[i];
        inputInPhaseSignal[i] = temp;
        phaseError = (-1+2*signs(inputInPhaseSignal[i] * inputQuadratureSignal[i])) * cos(timingClockPhase);
        lower = pllParams.G2*phaseError + lower;
        loopFilt = pllParams.G1*phaseError + lower;
        dco = dco + loopFilt;
    }

    toff[0] = floor(timingAngleLog[0] + 0.5);
    for (uint16_t i = 1; i < signalLength; i++){
        // The difference of of consecutive angles must be smaller than π
        toff[i] = floor(timingAngleLog[i - 1] + M_PI * floor((timingAngleLog[i] - timingAngleLog[i - 1]) / M_PI) + 0.5);
    }
    
    uint16_t k = 0;
    for (uint16_t i = 2*samplesPerSymbol; i < signalLength; i += 2 * samplesPerSymbol){
        if ((i - toff[i] + samplesPerSymbol) < signalLength) {
            inputInPhaseSignal[k] = inputInPhaseSignal[i - static_cast<int16_t>(toff[i])];
            inputQuadratureSignal[k] = inputQuadratureSignal[i - static_cast<int16_t>(toff[i]) + samplesPerSymbol];
        }
        k += 1;
    }

    for (uint16_t i = 0; i < symbolsN/2; i++){
        signal[2*i] = inputInPhaseSignal[i] > 0;
        signal[2*i+1] = inputQuadratureSignal[i] > 0;
    }

}

template class GMSKTranscoder<4>;
template class GMSKTranscoder<6>;
template class GMSKTranscoder<10>;