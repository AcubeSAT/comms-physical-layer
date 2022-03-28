
#ifndef MCU_IMPLEMENTATION_OQPSK_HPP
#define MCU_IMPLEMENTATION_OQPSK_HPP

#include "stdint.h"
#include "math.h"
#define ARM_MATH_CM4
#include "arm_math.h"
constexpr uint16_t blockLen = 8*2+2;
constexpr float32_t oqpsk_taps[23]={-0.0020520018879324198, -0.0003224409883841872, 0.002508002333343029,
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
    float firStateF32[23 + blockLen - 1];
    uint32_t sampling_frequency;
    uint32_t sampling_rate;
    uint32_t samples;
    uint8_t bits_per_sample = 2;
    uint32_t samples_per_symbol;
    float32_t internal_buffer[1 << 12]; // TODO: Determine size (signal_length * max_samples_per_symbol)
    arm_fir_instance_f32 armFIRInstanceF32;

public:
    OQPSKTranscoder(uint32_t sample_rate) : sampling_frequency(4 * sample_rate), sampling_rate(sample_rate) {
        samples_per_symbol = sampling_frequency /
                             sampling_rate; // samples_per_symbol and samples_per_symbol/2 must be integer values - throw error?
        arm_fir_init_f32(&armFIRInstanceF32, 23, const_cast<float32_t *>(oqpsk_taps), firStateF32, blockLen);

    }

    void modulate(uint8_t *signal, uint16_t signal_length, float32_t *in_phase_signal, float32_t *quadrature_signal);

};

#endif //MCU_IMPLEMENTATION_OQPSK_HPP
