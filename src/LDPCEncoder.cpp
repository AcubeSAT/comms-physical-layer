#include "LDPCEncoder.hpp"
#include <cstdint>
#include <etl/bitset.h>

LDPCEncoder::LDPCEncoder() = default;

void LDPCEncoder::encode(const etl::bitset<inputLength>& input, etl::bitset<outputLength>& output) {
    etl::bitset<errorCorrectionLength> errorCorrectionBits;
    for (int row = 0; row < generatorRows; row++) {
        for (int column = 0; column < generatorColumns; column++) {
            uint16_t index = row * generatorColumns + column;
            uint16_t thisParityPosition = positionRows[index];
            uint16_t lengthOfParity;
            if (thisParityPosition != positionRows[totalQuasiCyclicMatrices - 1]) {
                uint16_t nextParityPosition = positionRows[index + 1];
                lengthOfParity = nextParityPosition - thisParityPosition;
            } else {
                lengthOfParity = sizeParity - thisParityPosition;
            }
            for (int i = 0; i < lengthOfParity; i++) {
                uint16_t initialParity =
                        rowsParityBits[thisParityPosition + i] - column * sizeSquareCyclicMatrix;
                for (int j = 0; j < sizeSquareCyclicMatrix; j++) {
                    uint16_t updatedParity = initialParity + j;
                    if (updatedParity >= sizeSquareCyclicMatrix) {
                        updatedParity -= sizeSquareCyclicMatrix;
                    }
                    uint16_t position = column * sizeSquareCyclicMatrix + updatedParity;
                    uint8_t result = errorCorrectionBits[position] ^ input[row * sizeSquareCyclicMatrix + j];
                    errorCorrectionBits.set(position, result);
                }
            }
        }
    }
    for (int i = 0; i < encodingIterations; i++) {
        if (i < thresholdIndex) {
            output.set(i, input[i]);
        } else {
            output.set(i, errorCorrectionBits[i - 4096]);
        }
    }
}
