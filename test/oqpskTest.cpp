#include "GMSKTranscoder.hpp"
#include "OQPSKTranscoder.hpp"
#include "catch2/catch_all.hpp"
#include <fstream>
#include <iostream>

#define PACKET_LENGTH 8
#define NUM_PACKETS 5
#define PACKETS_TO_IGNORE 2

uint8_t oqpsk_packet[PACKET_LENGTH] = {1, 0, 1, 0, 0, 1, 0, 1};

uint8_t oqpsk_in[NUM_PACKETS*PACKET_LENGTH] = {0};
double oqpsk_iout[10*PACKET_LENGTH*NUM_PACKETS] = {0};  // Max samples per symbol tested = 10
double oqpsk_qout[10*PACKET_LENGTH*NUM_PACKETS] = {0};
bool demod_oqpsk_signal[NUM_PACKETS*PACKET_LENGTH] = {0};
double oqpsk_test[] = {-0.00145098, 0, -0.00167898, 0, 9.44408e-05, 0.00145098, -0.00348842, 0.00167898, 0.000311778, -9.44408e-05, 0.00138664, 0.00348842, -0.00393364, -0.000311778, 0.0111144, -0.00138664, -0.012585, 0.00393364, -0.0497491, -0.0111144, 0.151864, 0.012585, 0.56203, 0.0497491, 0.75959, -0.151864, 0.71982, -0.56203, 0.70386, -0.75959, 0.688812, -0.71982, 0.733792, -0.70386, 0.809651, -0.688812, 0.406678, -0.733792, -0.410072, -0.809651, -0.811018, -0.406678, -0.733857, 0.410072, -0.692745, 0.811018, -0.692745, 0.733857, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.732406, -0.692745, 0.809339, -0.692745, 0.410166, -0.732406, -0.410166, -0.809339, -0.809339, -0.410166, -0.732406, 0.410166, -0.692745, 0.809339, -0.692745, 0.732406, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.732406, -0.692745, 0.809339, -0.692745, 0.410166, -0.732406, -0.410166, -0.809339, -0.809339, -0.410166, -0.732406, 0.410166, -0.692745, 0.809339, -0.692745, 0.732406, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.732406, -0.692745, 0.809339, -0.692745, 0.410166, -0.732406, -0.410166, -0.809339, -0.809339, -0.410166, -0.732406, 0.410166, -0.692745, 0.809339, -0.692745, 0.732406, -0.732406, 0.692745, -0.809339, 0.692745, -0.410166, 0.732406, 0.410166, 0.809339, 0.809339, 0.410166, 0.732406, -0.410166, 0.692745, -0.809339, 0.692745, -0.732406, 0.733857, -0.692745, 0.811018, -0.692745};

TEST_CASE("OQPSK Modulation: SPS = 4") {
    int sps = 4;
    int sample_frequency = 48000;
    int symbol_rate = sample_frequency / sps;
    OQPSKTranscoder oqpsk_mod(sample_frequency, symbol_rate);

    SECTION("Repeated Packet") {
        // Add NUM_PACKETS packets to the input signal
        for (int i = 0; i < NUM_PACKETS; i++) {
            for (int j = 0; j < PACKET_LENGTH; j++) {
                oqpsk_in[PACKET_LENGTH * i + j] = oqpsk_packet[j];
            }
        }

        // Modulation of the input signal
        oqpsk_mod.modulate(oqpsk_in, NUM_PACKETS * PACKET_LENGTH, oqpsk_iout, oqpsk_qout);

        // Test that I/Q match the test values
        double error = 0.001;
        for(int i = 0; i < NUM_PACKETS * PACKET_LENGTH; i++){
            CHECK(std::abs(oqpsk_iout[i] - oqpsk_test[2*i]) <= error);
            CHECK(std::abs(oqpsk_qout[i] - oqpsk_test[2*i + 1]) <= error);
        }
        
        // Writing I/Q data to a file for plotting
        std::fstream iqFile;
        iqFile.open("../tests/iofiles/iqOQPSKrep.txt", std::ios::out);
        if (iqFile.is_open()) {
            for (int i = 0; i < sps * PACKET_LENGTH * NUM_PACKETS; i++) {
                iqFile << oqpsk_iout[i] << " " << oqpsk_qout[i] << " ";
            }
        }
    }

    // Random input to check the I/Q phasor plot form
    SECTION("Random Input"){
        // Add NUM_PACKETS packets to the input signal
        std::srand(std::time(nullptr));
        for (int i = 0; i < NUM_PACKETS; i++) {
            for (int j = 0; j < PACKET_LENGTH; j++) {
                oqpsk_in[PACKET_LENGTH * i + j] = std::rand() % 2;
            }
        }

        // Modulation of the input signal
        oqpsk_mod.modulate(oqpsk_in, NUM_PACKETS * PACKET_LENGTH, oqpsk_iout, oqpsk_qout);

        // Writing I/Q data to a file for plotting
        std::fstream iqFile;
        iqFile.open("../tests/iofiles/iqOQPSKrand.txt", std::ios::out);
        if (iqFile.is_open()) {
            for (int i = 0; i < sps * PACKET_LENGTH * NUM_PACKETS; i++) {
                iqFile << oqpsk_iout[i] << " " << oqpsk_qout[i] << " ";
            }
        }
    }
}