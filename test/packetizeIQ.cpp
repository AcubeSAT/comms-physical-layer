#include "catch2/catch_all.hpp"
#include "filter.hpp"

TEST_CASE("Packetization") {
    double packetI[] = {0.25, 1, -0.5, -1, 0.25};
    double packetQ[] = {0, -1, 1, 0.5, 0.25};
    uint32_t packetOut[5];

    packetizeIQ(packetI, packetQ, 5, packetOut);

    CHECK(packetOut[0] == 0b10000111111111110100000000000000);
    CHECK(packetOut[1] == 0b10011111111111110110000000000001);
    CHECK(packetOut[2] == 0b10110000000000010101111111111111);
    CHECK(packetOut[3] == 0b10100000000000010100111111111111);
    CHECK(packetOut[4] == 0b10000111111111110100011111111111);
}