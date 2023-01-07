#pragma once
#include <type_traits>
#include <cstdint>
class Channel {

  public:
    template<typename U>
    static constexpr std::enable_if_t<std::is_floating_point_v<U>, U> Pi = 3.141592653589793238462643383279502884L;

    static void addWGN(double* inPhase, double* quadrature, double snrdB, uint32_t symbolRate, uint8_t samplesPerSymbol, uint16_t numPackets, uint16_t packetLength);
    static void shiftPhase(double* inPhase, double* quadrature, uint8_t samplesPerSymbol, uint16_t numPackets, uint16_t packetLength, double phaseShiftRad);
    static void shiftFrequency(double* inPhase, double* quadrature, uint8_t samplesPerSymbol, uint16_t numPackets, uint16_t packetLength, double frequencyShiftHz, double samplingFrequency);
};
