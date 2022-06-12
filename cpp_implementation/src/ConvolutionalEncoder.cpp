#include "ConvolutionalEncoder.hpp"

ConvolutionalEncoder::ConvolutionalEncoder() {
    inputItemsNumber = encodedCodewordLength / rate;
}

void ConvolutionalEncoder::encodeMessage(const bool* const inputMessageAddress, bool* outputMessageAddress) {

    for (uint8_t generatorBit = 1; generatorBit < constraintLength; generatorBit++) {
        for (uint8_t iGenerator = 0; iGenerator < rate; iGenerator++) {
            for (uint8_t stateBit = 0; stateBit < generatorBit; stateBit++) {
                *outputMessageAddress ^= inputMessageAddress[stateBit] * generator[iGenerator][constraintLength - generatorBit + stateBit];
            }
            outputMessageAddress++;
        }
    }

    for(int generatorBit = 0; generatorBit < inputItemsNumber - (constraintLength - 1); generatorBit++) {
        for (uint8_t iGenerator = 0; iGenerator < rate; iGenerator++) {
            for (uint8_t stateBit = 0; stateBit < constraintLength; stateBit++) {
                *outputMessageAddress ^= inputMessageAddress[generatorBit + stateBit] * generator[iGenerator][stateBit];
            }
            outputMessageAddress++;
        }
    }
}

ConvolutionalEncoder::~ConvolutionalEncoder() = default;
