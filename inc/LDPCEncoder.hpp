#pragma once

#include <stdint.h>
#include <cstddef>
#include "PositionRowsEsoteric.hpp"
#include "RowsParityBitsEsoteric.hpp"

/**
 * LDPC encoder compliant to CCSDS specification.
 * https://public.ccsds.org/Pubs/231x0b4e0.pdf
 */
class LDPCEncoder {
private:
    /**
     * the number of rows with square quise-cyclic matricies in the Gnerator Matrix
     */
    static const uint8_t generatorRows = 32;

    /**
     * the number of columns with square quisi-cyclic matricies in the Generator Matrix
     */
    static const uint8_t generatorColumns = 8;

    /**
     * the size of each square quasi-cyclic matrix
     */
    static const uint8_t sizeSquareCyclicMatrix = 128;

    static const uint16_t sizeParity = 16362;

    /**
     * the position of the Parity Bits in the Generator Matrix (Identity matrix excluded). Since the Generator Matrix
     * consists of square cyclic block matrices, only the parities in the first line of each sub-matrix have been kept
     */
    inline static const uint16_t* rowsParityBits = rowsParityBitsEsoteric;
    /**
     * the total number of square quasi-cyclic matrices starting from 0
     */
    static const int totalQuasiCyclicMatrices = generatorColumns * generatorRows;

    /**
     * the positions in vector rowsParityBits where the parities belong to a new block matrix.
     */

    inline static const uint16_t* positionRows = positionRowsEsoteric;

    /**
     * size of the initial message
     */
    static const int sizeInitialMessage = 4096;

    /**
     * size of the encoded message.
     */
    static const int sizeEncodedMessage = generatorColumns * sizeSquareCyclicMatrix;

    /**
     * size of the full message that will be transited consisting of the initial message and the encoded message
     */
    static const int sizeMessage = sizeInitialMessage + sizeEncodedMessage;

    /**
     * number of iterations in the loop applying the encoding on the data
     */
     static constexpr size_t encodingIterations = 5120;

     /**
      * number of indices, in which the output is just the input
      */
     static constexpr size_t thresholdIndex = 4096;


public:

    /**
     * Class initialiser. Sets the output to be multiple of sizeInitialMessage
     */
    LDPCEncoder();

    /**
     * The encoding of the initial message
     * @param noutput_items The number of output bits
     * @param ninput_items The number of input bits on each port
     * @param input_items The input buffer
     * @param output_items The output buffer
     * @return noutput_items
     */
    void encode(const bool* inputMessage, bool* outputMessage);

};