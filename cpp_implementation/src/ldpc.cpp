#include <cstdint>
#include "ldpc.hpp"

ldpc::ldpc() = default;

void ldpc::encoder(const bool *inputMessage, bool *outputMessage) {

    bool encodedMessage[sizeEncodedMessage];
    for (bool &i: encodedMessage) {
        i = false;
    }
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
                    encodedMessage[column * sizeSquareCyclicMatrix + updatedParity] ^= inputMessage[row * sizeSquareCyclicMatrix + j];
                }
            }
        }
    }
    for (int i = 0; i < encodingIterations; i++) {
        if (i < thresholdIndex) {
            outputMessage[i] = inputMessage[i];
        } else {
            outputMessage[i] = encodedMessage[i - 4096];
        }
    }
}
