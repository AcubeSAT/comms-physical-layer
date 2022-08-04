#include "ConvolutionalEncoder.hpp"

ConvolutionalEncoder::ConvolutionalEncoder() {
    inputItemsNumber = encodedCodewordLength / rate;
}

void ConvolutionalEncoder::encodeMessage(const bool* const inputMessageAddress, bool* const outputMessageAddress) {
    for (uint16_t i = 0; i < encodedCodewordLength; ++i) {
        outputMessageAddress[i] = 0;
    }
    for (uint8_t generatorBit = 1; generatorBit < constraintLength; generatorBit++) {
        for (uint8_t iGenerator = 0; iGenerator < rate; iGenerator++) {
            for (uint8_t stateBit = 0; stateBit < generatorBit; stateBit++) {
                outputMessageAddress[(generatorBit - 1) * rate + iGenerator] ^= inputMessageAddress[stateBit] * generator[iGenerator][constraintLength - generatorBit + stateBit];
            }
        }
    }
    for(int generatorBit = 0; generatorBit < inputItemsNumber - (constraintLength - 1); generatorBit++) {
        for (uint8_t iGenerator = 0; iGenerator < rate; iGenerator++) {
            for (uint8_t stateBit = 0; stateBit < constraintLength; stateBit++) {
                outputMessageAddress[(constraintLength - 1) * rate + generatorBit*rate + iGenerator] ^= inputMessageAddress[generatorBit + stateBit] * generator[iGenerator][stateBit];
            }
        }
    }
}

ConvolutionalEncoder::~ConvolutionalEncoder() = default;
