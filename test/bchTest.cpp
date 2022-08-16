#include "BCHDecoder.hpp"
#include "catch2/catch_all.hpp"

/**
 * BCH Decoding
 * ---
 * `bchTest.cpp` includes tests for the BCH decoder.
 * - BCH-Test no1: Decoding a single codeword with a single error successfully.
 * - BCH-Test no2: Decoding another codeword with a single error successfully.
 * - BCH-Test no3: Decoding another single codeword successfully, since it doesn't contain any errors.
 * - BCH-Test no4: Decoding a word with 2 errors, decoder should return its empty variant to mark a failure.
 *
 * `resultData.value()` is shifted by 8 to the right because the value returned from the decoder is a 56 bit word.
 */
TEST_CASE("BCH decoder - Fixed words") {
    BCHDecoder decoder { };
    SECTION("BCH-Test no1: Word with 1 error - Decoder succeeds") {
        uint64_t data = 0b1110101010101010101010101010101010101010101010101010101001010000;
        auto resultData = decoder.decodeBCH(data);
        REQUIRE(resultData.has_value());
        constexpr uint64_t expectedResult = 0b10101010101010101010101010101010101010101010101010101010;
        CHECK(resultData.value() == expectedResult);
    }
    SECTION("BCH-Test no2: Another word with 1 error - Decoder succeeds") {
        uint64_t data = 0b1011101010101010101010101010101010101010101010101010101001010000;
        auto resultData = decoder.decodeBCH(data);
        constexpr uint64_t expectedResult = 0b10101010101010101010101010101010101010101010101010101010;
        REQUIRE(resultData.has_value());
        CHECK(resultData.value() == expectedResult);
    }
    SECTION("BCH-Test no3: Word with no errors - Decoder succeeds") {
        uint64_t data = 0b1010101010101010101010101010101010101010101010101010101001010000;
        auto resultData = decoder.decodeBCH(data);
        REQUIRE(resultData.has_value());
        constexpr uint64_t expectedResult = 0b10101010101010101010101010101010101010101010101010101010;
        CHECK(resultData.value() == expectedResult);
    }
    SECTION("BCH-Test no4: Word with 2 errors - Decoder fails") {
        uint64_t data = 0b1111101010101010101010101010101010101010101010101010101001010000;
        auto resultData = decoder.decodeBCH(data);
        REQUIRE(!resultData.has_value());
    }
}
