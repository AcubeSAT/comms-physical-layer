
#ifndef MCU_IMPLEMENTATION_OQPSK_HPP
#define MCU_IMPLEMENTATION_OQPSK_HPP

#include <stdint.h>
#include "math.h"

constexpr double oqpsk_taps[23]{-0.0020520018879324198, -0.0003224409883841872, 0.002508002333343029,
                                -0.00506692985072732, 0.005374290514737368,
                                0.0015200789785012603, -0.007524007000029087, 0.021281106397509575,
                                -0.03762003779411316, -0.05320276319980621, 0.2901403307914734,
                                0.5699287056922913, 0.2901403307914734, -0.05320276319980621, -0.03762003779411316,
                                0.021281106397509575,
                                -0.007524007000029087, 0.0015200789785012603, 0.005374290514737368,
                                -0.00506692985072732, 0.002508002333343029,
                                -0.0003224409883841872, -0.0020520018879324198};

class OQPSKTranscoder {

private:
    uint32_t sampling_frequency;
    uint32_t sampling_rate;
    uint32_t samples;
    uint8_t bits_per_sample = 2;
    uint32_t samples_per_symbol;
    double internal_buffer[1 << 12]; // TODO: Determine size (signal_length * max_samples_per_symbol)

public:
    OQPSKTranscoder(uint32_t sample_rate) : sampling_frequency(4 * sample_rate), sampling_rate(sample_rate) {
        samples_per_symbol = sampling_frequency /
                             sampling_rate; // samples_per_symbol and samples_per_symbol/2 must be integer values - throw error?
    }

    void modulate(uint8_t *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal);

};

#endif //MCU_IMPLEMENTATION_OQPSK_HPP
