#pragma once

#include <stdint.h>
#include <cstddef>
#include "etl/bitset.h"
#include "PositionRowsEsoteric.hpp"
#include "RowsParityBitsEsoteric.hpp"

/**
 * LDPC encoder compliant to CCSDS specification.
 * https://public.ccsds.org/Pubs/231x0b4e0.pdf
 */
class LDPCEncoder {
    public:
    /**
     * the number of rows with square quasi-cyclic matrices in the Generator Matrix
     */
    static constexpr uint8_t generatorRows = 32;
    /**
     * the number of columns with square quasi-cyclic matrices in the Generator Matrix
     */
    static constexpr uint8_t generatorColumns = 8;
    /**
     * the size of each square quasi-cyclic matrix
     */
    static constexpr uint8_t sizeSquareCyclicMatrix = 128;

    static constexpr uint16_t sizeParity = 16362;
    /**
     * the position of the Parity Bits in the Generator Matrix (Identity matrix excluded). Since the Generator Matrix
     * consists of square cyclic block matrices, only the parities in the first line of each sub-matrix have been kept
     */
    inline static const uint16_t* rowsParityBits = rowsParityBitsEsoteric;
    /**
     * the total number of square quasi-cyclic matrices starting from 0
     */
    static constexpr uint16_t totalQuasiCyclicMatrices = generatorColumns * generatorRows;
    /**
     * the positions in vector rowsParityBits where the parities belong to a new block matrix.
     */
    inline static const uint16_t* positionRows = positionRowsEsoteric;
    /**
     * size of the initial message
     */
    static constexpr uint16_t inputLength = 4096;
    /**
     * number of Error Correction bits which will be appended to the input.
     */
    static constexpr uint16_t errorCorrectionLength = generatorColumns * sizeSquareCyclicMatrix;
    /**
     * size of the full message that will be transited consisting of the initial message and the encoded message
     */
    static constexpr uint16_t outputLength = inputLength + errorCorrectionLength;
    /**
     * number of iterations in the loop applying the encoding on the data
     */
    static constexpr size_t encodingIterations = 5120;
    /**
     * number of indices, in which the output is just the input
     */
    static constexpr size_t thresholdIndex = 4096;
    /**
     * Class initializer. Sets the output to be multiple of inputLength
     */
    LDPCEncoder();
    /**
     * The encoding method
     * @param input Reference to bit packed byte-encoded data (etl::<bitset>) of size inputLength
     * @param output Reference to bit packed byte-encoded data (etl::<bitset>) of size outputLength
     */
    void encode(const etl::bitset<inputLength>& input, etl::bitset<outputLength>& output);
};