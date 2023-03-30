#include "OQPSKTranscoder.hpp"
#include "catch2/catch_all.hpp"
#include <fstream>
#include <iostream>

TEST_CASE("OQPSK Modulation") {
    static constexpr uint8_t packetLength = 8;
    static constexpr uint8_t numOfPackets = 5;
    static constexpr double expectedResult[] = {-0.00145098, 0, -0.00167898, 0, 9.44408e-05, 0.00145098, -0.00348842, 0.00167898, 0.000311778, -9.44408e-05, 0.00138664, 0.00348842, -0.00393364, -0.000311778, 0.0111144, -0.00138664, -0.012585, 0.00393364, -0.0497491, -0.0111144, 0.151864, 0.012585, 0.56203, 0.0497491, 0.75959, -0.151864, 0.71982, -0.56203, 0.70386, -0.75959, 0.688812, -0.71982, 0.733792, -0.70386, 0.809651, -0.688812, 0.406678, -0.733792, -0.410072, -0.809651, -0.811018, -0.406678, -0.733857, 0.410072, -0.692745, 0.811018, -0.692745, 0.733857, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.732406, -0.692745, 0.809339, -0.692745, 0.410166, -0.732406, -0.410166, -0.809339, -0.809339, -0.410166, -0.732406, 0.410166, -0.692745, 0.809339, -0.692745, 0.732406, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.732406, -0.692745, 0.809339, -0.692745, 0.410166, -0.732406, -0.410166, -0.809339, -0.809339, -0.410166, -0.732406, 0.410166, -0.692745, 0.809339, -0.692745, 0.732406, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.732406, -0.692745, 0.809339, -0.692745, 0.410166, -0.732406, -0.410166, -0.809339, -0.809339, -0.410166, -0.732406, 0.410166, -0.692745, 0.809339, -0.692745, 0.732406, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.733857, -0.692745, 0.811018, -0.692745};
    static constexpr uint32_t symbolRate = 12000;
    OQPSKTranscoder<packetLength*numOfPackets> oqpsk(symbolRate);

    float inPhaseSignal[oqpsk.samplesPerSymbol*packetLength*numOfPackets] = {0};
    float quadratureSignal[oqpsk.samplesPerSymbol*packetLength*numOfPackets] = {0};

    SECTION("Repeated Packet") {
        // Add numOfPackets packets to the input signal
        // The packet used is 10100101
        const etl::bitset<packetLength * numOfPackets> input = 0b1010010110100101101001011010010110100101;

        // Modulation of the input signal
        oqpsk.modulate(input, inPhaseSignal, quadratureSignal);

        // Test that I/Q match the test values
        double error = 0.001;
        for(int i = 0; i < numOfPackets * packetLength; i++){
            CHECK(std::abs(inPhaseSignal[i] - expectedResult[2*i]) <= error);
            CHECK(std::abs(quadratureSignal[i] - expectedResult[2*i + 1]) <= error);
        }
        
        // Writing I/Q data to a file for plotting
        std::fstream iqFile;
        iqFile.open("../test/iofiles/out_iqOQPSKrep.txt", std::ios::out);
        if (iqFile.is_open()) {
            for (int i = 0; i < oqpsk.samplesPerSymbol * packetLength * numOfPackets; i++) {
                iqFile << inPhaseSignal[i] << " " << quadratureSignal[i] << " ";
            }
        }
    }

    // Random input to check the I/Q phasor plot form
    SECTION("Random Input"){
        // Add numOfPackets packets to the input signal
        const etl::bitset<packetLength * numOfPackets> input = 0b1101001110100100000101001011010101010000;

        // Modulation of the input signal
        oqpsk.modulate(input, inPhaseSignal, quadratureSignal);

        // Writing I/Q data to a file for plotting
        std::fstream iqFile;
        iqFile.open("../test/iofiles/out_iqOQPSKrand.txt", std::ios::out);
        if (iqFile.is_open()) {
            for (int i = 0; i < oqpsk.samplesPerSymbol * packetLength * numOfPackets; i++) {
                iqFile << inPhaseSignal[i] << " " << quadratureSignal[i] << " ";
            }
        }
    }
}