#pragma once

#include "etl/bitset.h"

template <uint16_t inputLength> class OQPSKTranscoder {
  public:
    /**
     * @param samplesPerSymbol sps is hardcoded to 4
     */
    static constexpr uint8_t samplesPerSymbol = 4;
    /**
     * @param samplingFrequency No. of samples / second
     */
    uint32_t samplingFrequency;
    /**
     * @param symbolRate No. of symbols / second
     */
    uint32_t symbolRate;
    /**
     * @param numberOfSymbols The number of symbols which will be generated.
     * It equals input length divided by two, because OQPSK codes one symbol for 2 bits at the same time (4 positions).
     */
    static constexpr uint16_t numberOfSymbols = inputLength / 2;

    float internalBuffer[inputLength * samplesPerSymbol];
    /**
     * @param numOQPSKTaps number of taps is hardcoded to 23
     */
    static constexpr uint8_t numOQPSKTaps = 23;
    /**
     * @param oqpskTaps
     */
    float oqpskTaps[numOQPSKTaps] = {-0.0020520018879324198, -0.0003224409883841872, 0.002508002333343029, -0.00506692985072732, 0.005374290514737368,
                          0.0015200789785012603, -0.007524007000029087, 0.021281106397509575, -0.03762003779411316, -0.05320276319980621, 0.2901403307914734,
                          0.5699287056922913, 0.2901403307914734, -0.05320276319980621, -0.03762003779411316, 0.021281106397509575,
                          -0.007524007000029087, 0.0015200789785012603, 0.005374290514737368, -0.00506692985072732, 0.002508002333343029,
                          -0.0003224409883841872, -0.0020520018879324198};

    OQPSKTranscoder(uint32_t symbolRate): symbolRate(symbolRate), samplingFrequency(symbolRate * samplesPerSymbol){}
    /**
     * The modulating method
     * @param input
     * @param inPhaseSignal Serves as output for the in phase signal. Should be allocated before calling the method
     * @param quadratureSignal Serves as output for the quadrature signal. Should be allocated before calling the method
     */
    void modulate(const etl::bitset<inputLength>& input, float *inPhaseSignal, float *quadratureSignal);
};