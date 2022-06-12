#include "BCHDecoder.hpp"

BCHDecoder::BCHDecoder(){}

BCHDecoder::~BCHDecoder(){}

etl::pair<uint64_t, uint64_t> BCHDecoder::binaryPolynomialDivision(uint64_t dividend, uint64_t divisor) {
    if (divisor == 0) {
        return {UINT64_MAX, 0};
    }
    if (dividend == 0) {
        return {0, divisor};
    }

    uint64_t dividendDegree = static_cast<uint64_t>(floor(log2(dividend))) + 1;
    uint64_t divisorDegree = static_cast<uint64_t>(floor(log2(divisor))) + 1;
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
    uint64_t degR = static_cast<uint64_t>(floor(log2(dividend))) + 1;
    return {dividend, quotient << (degR - 1)};
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

std::optional<uint64_t> BCHDecoder::decodeBCH(uint64_t codeword) {
    uint64_t syndrome1 = calculateSyndromes(codeword).first;
    if (syndrome1 == 0) {
        return codeword;
    } else if (syndromesHash.contains(syndrome1)) {
        uint64_t const exponentOfTwo = 63 - syndromesHash.at(syndrome1) - 1;
        codeword ^= UINT64_C(1) << exponentOfTwo;
        return codeword;
    }
    return {};
}