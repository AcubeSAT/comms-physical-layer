#include "ConvolutionalEncoder.hpp"
#include "catch2/catch_all.hpp"
#include <fstream>

TEST_CASE("Convolutional Encoder Works") {
    ConvolutionalEncoder convolutional;
    const uint wordSize = 2048;
    bool src[wordSize];
    bool expectedDst[2*wordSize];
    bool dst[2*wordSize];

    SECTION("Zeros") {
        for (int i = 0; i < wordSize; i++) {
            src[i] = 0;
        }
        convolutional.encodeMessage(src, dst);

        uint16_t errors = 0;
        for (int i = 0; i < 2*wordSize; i++) {
            if (dst[i] != 0) {
                errors += 1;
            }
        }
        CHECK(errors == 0);
    }
    SECTION("Random Word") {
        std::ifstream srcFile("../test/iofiles/convolutional_src_data");
        for (int i = 0; i < wordSize; i++) {
            srcFile >> src[i];
        }


        std::ifstream dstFile("../test/iofiles/convolutional_dst_data");
        for (int i = 0; i < 2*wordSize; i++) {
            dstFile >> expectedDst[i];
        }

        convolutional.encodeMessage(src, dst);

        uint16_t errors = 0;
        for (int i = 0; i < 2*wordSize; i++) {
            if (dst[i] != expectedDst[i]) {
                errors += 1;
            }
        }


            CHECK(errors == 0);
    }

}

