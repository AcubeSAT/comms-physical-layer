#include "BCHDecoder.hpp"
#include "etl/bit.h"

const etl::map<uint8_t, uint8_t, 63> BCHDecoder::syndromesHash {
        {98, 0}  , {49, 1} , {122, 2} , {61, 3}  , {124, 4} , {62, 5}
      , {31, 6}  , {109, 7}, {84, 8}  , {42, 9}  , {21, 10} , {104, 11}
      , {52, 12} , {26, 13}, {13, 14} , {100, 15}, {50, 16} , {25, 17}
      , {110, 18}, {55, 19}, {121, 20}, {94, 21} , {47, 22} , {117, 23}
      , {88, 24} , {44, 25}, {22, 26} , {11, 27} , {103, 28}, {81, 29}
      , {74, 30} , {37, 31}, {112, 32}, {56, 33} , {28, 34} , {14, 35}
      , {7, 36}  , {97, 37}, {82, 38} , {41, 39} , {118, 40}, {59, 41}
      , {127, 42}, {93, 43}, {76, 44} , {38, 45} , {19, 46} , {107, 47}
      , {87, 48} , {73, 49}, {70, 50} , {35, 51} , {115, 52}, {91, 53}
      , {79, 54} , {69, 55}, {64, 56} , {32, 57} , {16, 58} , {8, 59}
      , {4, 60}  , {2, 61} , {1, 62}
};

etl::pair<uint64_t, uint64_t> BCHDecoder::binaryPolynomialDivision(uint64_t dividend, uint64_t divisor) {
    if (divisor == 0) {
        return {UINT64_MAX, 0};
    }
    if (dividend == 0) {
        return {0, divisor};
    }

    uint64_t dividendDegree = etl::bit_width(dividend);
    uint64_t divisorDegree = etl::bit_width(divisor);
    size_t bitPosition = 0;
    uint64_t quotient = 0;
    while (dividend >= divisor) {
        quotient <<= 1;
        if ((dividend >> (dividendDegree - bitPosition - 1) & 1) == 1) {
            dividend ^= divisor << (dividendDegree - divisorDegree - bitPosition);
            quotient++;
        }
        bitPosition++;
    }

    return {dividend, quotient << (etl::bit_width(dividend) - 1)};
}

etl::pair<uint64_t, uint64_t> BCHDecoder::calculateSyndromes(uint64_t receivedMessage) {
    uint64_t syndrome1 = 0, syndrome2 = 0;
    size_t index = 0;
    while (receivedMessage) {
        auto currentBit = receivedMessage & 1;
        if (currentBit == 1) {
            syndrome1 ^= primitiveElements[index % 63];
            syndrome2 ^= primitiveElements[(2 * index) % 63];
        }
        index++;
        receivedMessage >>= 1;
    }
    syndrome1 = binaryPolynomialDivision(syndrome1, generatorPolynomial).first;
    syndrome2 = binaryPolynomialDivision(syndrome2, generatorPolynomial).first;

    return {syndrome1, syndrome2};
}

[[nodiscard]] etl::optional<uint64_t> BCHDecoder::decodeBCH(uint64_t codeword) {
    uint64_t syndrome1 = calculateSyndromes(codeword).first;
    if (syndrome1 == 0) {
        // Shift the codeword to get rid of the error control bits
        return codeword >> parityAndDummyBits;
    } else if (syndromesHash.contains(syndrome1)) {
        uint64_t const exponentOfTwo = 63 - syndromesHash.at(syndrome1) - 1;
        codeword ^= UINT64_C(1) << exponentOfTwo;
        // Like above, shift the codeword to get rid of the error control bits
        return codeword >> parityAndDummyBits;
    }
    return {};
}