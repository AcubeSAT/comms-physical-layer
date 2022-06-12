#ifndef TC_SYNCHRONIZATION_BCHDECODER_HPP
#define TC_SYNCHRONIZATION_BCHDECODER_HPP

#include <cstdint>
#include <optional>
#include "etl/map.h"

class BCHDecoder {

public:
    BCHDecoder();
    ~BCHDecoder();

    /**
     * @param generatorPolynomial The generator polynomial g(x) = x^7 + x^6 + x^2 + 1
     */
    static constexpr uint8_t generatorPolynomial = 0b011000101;

    /**
     * https://public.ccsds.org/Pubs/231x0b4e0.pdf
     * CCSDS spec defines a BCH(63,56) ECC algorithm
     */
    static constexpr uint64_t n = 63;
    static constexpr uint64_t k = 56;

    /**
     * @param primitiveElements The lookup table used to speed up the calculation of the syndromes
     */
    static constexpr uint8_t primitiveElements[64] { 1 , 2 , 4  , 8  , 16 , 32, 64 , 69
                                                   , 79, 91, 115, 35 , 70 , 73, 87 , 107
                                                   , 19, 38, 76 , 93 , 127, 59, 118, 41
                                                   , 82, 97, 7  , 14 , 28 , 56, 112, 37
                                                   , 74, 81, 103, 11 , 22 , 44, 88 , 117
                                                   , 47, 94, 121, 55 , 110, 25, 50 , 100
                                                   , 13, 26, 52 , 104, 21 , 42, 84 , 109
                                                   , 31, 62, 124, 61 , 122, 49, 98 , 1 };

    /**
     * @param syndromesHas The map used to match the syndrome with the error location.
     */
    static const etl::map<uint8_t, uint8_t, 63> syndromesHash;

    /**
     * Performs long polynomial division in GF(2) - essentially long binary division.
     * @param dividend The dividend of the operation
     * @param divisor The divisor of the operation
     * @return A pair containing the remainder of the operation and the quotient shifted left by its true degree.
     */
    etl::pair<uint64_t, uint64_t> binaryPolynomialDivision(uint64_t dividend, uint64_t divisor);
    /**
     * Calculates syndromes given the received message r for BCH(63, 56)
     * @param receivedMessage The received message r
     * @return A pair containing the syndromes.
     */
    etl::pair<uint64_t, uint64_t> calculateSyndromes(uint64_t receivedMessage);
    /**
     * Decodes a BCH codeword with at most 1 error using a map. For the general case, the Berlekamp-Massey
     * algorithm over a Galois field of characteristic 2 can be used. The problem is equivalent to
     * determining an LFSR of minimal degree that finds the coefficients for the linear sum of the
     * syndromes which results in 0. If there are no detectable errors, it returns the sent data.
     * If there is at most one error it corrects it and returns the sent data. If there are more than
     * one errors, it can't decode the codeword.
     * error correction code.
     * @param codeword The binary word to be decoded
     * @return The decoded message in case of a valid codeword, or a nullopt_t object otherwise
     */
    std::optional<uint64_t> decodeBCH(uint64_t codeword);

};

const etl::map<uint8_t, uint8_t, 63> BCHDecoder::syndromesHash { {98, 0}  , {49, 1} , {122, 2} , {61, 3}  , {124, 4} , {62, 5}
                                                               , {31, 6}  , {109, 7}, {84, 8}  , {42, 9}  , {21, 10} , {104, 11}
                                                               , {52, 12} , {26, 13}, {13, 14} , {100, 15}, {50, 16} , {25, 17}
                                                               , {110, 18}, {55, 19}, {121, 20}, {94, 21} , {47, 22} , {117, 23}
                                                               , {88, 24} , {44, 25}, {22, 26} , {11, 27} , {103, 28}, {81, 29}
                                                               , {74, 30} , {37, 31}, {112, 32}, {56, 33} , {28, 34} , {14, 35}
                                                               , {7, 36}  , {97, 37}, {82, 38} , {41, 39} , {118, 40}, {59, 41}
                                                               , {127, 42}, {93, 43}, {76, 44} , {38, 45} , {19, 46} , {107, 47}
                                                               , {87, 48} , {73, 49}, {70, 50} , {35, 51} , {115, 52}, {91, 53}
                                                               , {79, 54} , {69, 55}, {64, 56} , {32, 57} , {16, 58} , {8, 59}
                                                               , {4, 60}  , {2, 61} , {1, 62} };

#endif //TC_SYNCHRONIZATION_BCHDECODER_HPP
