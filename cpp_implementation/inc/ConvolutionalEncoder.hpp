#ifndef TC_SYNCHRONIZATION_CONVOLUTIONALENCODER_HPP
#define TC_SYNCHRONIZATION_CONVOLUTIONALENCODER_HPP

#include <stdint.h>

class ConvolutionalEncoder {
protected:
    /**
     * @param generator The generator polynomials used for the encoding, but inversed
     */
    inline static const bool generator[2][7] = {{1, 0, 0, 1, 1, 1, 1}, {1, 1, 0, 1, 1, 0, 1}};
    /**
     * @param rate The number of the generator polynomials
     */
    static const uint16_t rate = 2;
    /**
     * @param constraintLength The length of each generator polynomial
     */
    static const uint8_t constraintLength = 7;
    /**
     * @param decodedCodewordLength The length of the encoded codeword
     */
    static const uint16_t encodedCodewordLength = 4096;
    bool encodedMessage[encodedCodewordLength]{};

public:
    uint16_t inputItemsNumber;
    /**
     * Initializes an object of the class and sets the output to be a multiple of the encodedCodewordLength.
     */
    ConvolutionalEncoder();
    /*!
     * Destroys the object of the class.
     */
    ~ConvolutionalEncoder();

    void encodeMessage(const bool*inputMessageAddress, bool* outputMessageAddress);
};


#endif //TC_SYNCHRONIZATION_CONVOLUTIONALENCODER_HPP
