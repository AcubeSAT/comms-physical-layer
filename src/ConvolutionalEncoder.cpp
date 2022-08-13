#include "ConvolutionalEncoder.hpp"

ConvolutionalEncoder::ConvolutionalEncoder() {}

ConvolutionalEncoder::~ConvolutionalEncoder() = default;

void ConvolutionalEncoder::encode(const etl::bitset<inputLength>& input, etl::bitset<outputLength>& output) {
    for (uint16_t i = 0; i < outputLength; ++i) {
        output.set(i, 0);
    }
    uint16_t position;
    uint8_t result;
    for (uint8_t generatorBit = 1; generatorBit < constraintLength; generatorBit++) {
        for (uint8_t iGenerator = 0; iGenerator < rate; iGenerator++) {
            position = (generatorBit - 1) * rate + iGenerator;
            for (uint8_t stateBit = 0; stateBit < generatorBit; stateBit++) {
                result = output[position] ^ (input[stateBit] * generator[iGenerator][constraintLength - generatorBit + stateBit]);
                output.set(position , result);
            }
        }
    }
    for(int generatorBit = 0; generatorBit < inputLength - (constraintLength - 1); generatorBit++) {
        for (uint8_t iGenerator = 0; iGenerator < rate; iGenerator++) {
            position = (constraintLength - 1) * rate + generatorBit*rate + iGenerator;

            for (uint8_t stateBit = 0; stateBit < constraintLength; stateBit++) {
                result = output[position] ^ (input[generatorBit + stateBit] * generator[iGenerator][stateBit]);
                output.set(position , result);
            }
        }
    }
}