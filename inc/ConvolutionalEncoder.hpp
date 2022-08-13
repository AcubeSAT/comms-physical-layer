#pragma once

#include "etl/bitset.h"
#include <stdint.h>

class ConvolutionalEncoder {
  public:
    /**
     * @param rate The number of the generator polynomials
     */
    static constexpr uint8_t rate = 2;
    /**
     * @param constraintLength The length of each generator polynomial
     */
    static constexpr uint8_t constraintLength = 7;
    /**
     * @param generator The generator polynomials used for the encoding, but inversed
     */
    inline static const bool generator[rate][constraintLength] = {{1, 0, 0, 1, 1, 1, 1},
                                                                  {1, 1, 0, 1, 1, 0, 1}};
    /**
     * @param inputLength The length of the input
     */
    static constexpr uint16_t inputLength = 2048;
    /**
     * @param outputLength The length of the encoded codeword
     */
    static constexpr uint16_t outputLength = inputLength * rate;

    /**
     * Initializes an object of the class.
     */
    ConvolutionalEncoder();
    /**
     * Destroys the object of the class.
     */
    ~ConvolutionalEncoder();
    /**
     * The encoding method
     * @param input Reference to bit packed byte-encoded data (etl::<bitset>) of size inputLength
     * @param output Reference to bit packed byte-encoded data (etl::<bitset>) of size outputLength
     */
    void encode(const etl::bitset<inputLength>& input, etl::bitset<outputLength>& output);
};