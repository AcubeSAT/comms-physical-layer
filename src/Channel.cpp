#include "Channel.h"
#include <math.h>
#include <random>
#include <etl/random.h>

void Channel::addWGN(double *inPhase, double *quadrature, double snrdB,
                     uint32_t symbolRate, uint8_t samplesPerSymbol,
                     uint16_t numPackets, uint16_t packetLength) {

    uint32_t sampleFrequency = samplesPerSymbol * symbolRate;
    const double snr = pow(10, snrdB / 10);
    const double variance = sampleFrequency / (symbolRate * snr);

    std::default_random_engine generator{0};
    std::normal_distribution<double> dist(0, 1);


    for (int i = 0; i < samplesPerSymbol * numPackets * packetLength; i++) {
        inPhase[i] += sqrt(variance / 2) * dist(generator);
        quadrature[i] += sqrt(variance / 2) * dist(generator);
    }
}

void Channel::shiftPhase(double *inPhase, double *quadrature,
                         uint8_t samplesPerSymbol, uint16_t numPackets,
                         uint16_t packetLength, double phaseShiftRad) {

    // multiply x[n] with e^(j*phase) to phase shift
    for (int i = 0; i < samplesPerSymbol * numPackets * packetLength; i++) {
        double inPhaseTemp = inPhase[i];
        inPhase[i] = inPhase[i] * cos(phaseShiftRad) -
                     quadrature[i] * sin(phaseShiftRad);
        quadrature[i] = inPhaseTemp * sin(phaseShiftRad) +
                        quadrature[i] * cos(phaseShiftRad);
    }
}

void Channel::shiftFrequency(double *inPhase, double *quadrature,
                             uint8_t samplesPerSymbol, uint16_t numPackets,
                             uint16_t packetLength, double frequencyShiftHz,
                             double samplingFrequency) {

    // multiply x[n] with e^(2*pi*f*j*n) to shift the frequency as in FT
    double angle;
    for (int i = 0; i < samplesPerSymbol * numPackets * packetLength; i++) {
        angle = 2 * Pi<double> * frequencyShiftHz * i / samplingFrequency;
        double inPhaseTemp = inPhase[i];
        inPhase[i] = inPhaseTemp * cos(angle) - quadrature[i] * sin(angle);
        quadrature[i] = inPhaseTemp * sin(angle) + quadrature[i] * cos(angle);
    }
}