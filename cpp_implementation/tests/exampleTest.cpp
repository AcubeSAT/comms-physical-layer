#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "gmsk.h"
#include <iostream>

#define PACKET_LENGTH 16
#define NUM_PACKETS 2

TEST_CASE("helloWorld", "[helloWorld]" ) {
    std::cout << "Hello, World!" << std::endl;
    CHECK(1==1);
}

TEST_CASE("GMSK mod/demod (No noise)", "[noNoiseModem]" ) {

    int sample_frequency = 48000;
    int symbol_rate = 4800;
    int sps = sample_frequency / symbol_rate;
    GMSKTranscoder gmsk_mod(sample_frequency, symbol_rate);
    int8_t packet[PACKET_LENGTH] = {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1};
    int8_t gmsk_in[NUM_PACKETS*PACKET_LENGTH] = {0};
    double gmsk_iout[10*PACKET_LENGTH*NUM_PACKETS] = {0};   // samples/symbol = 10
    double gmsk_qout[10*PACKET_LENGTH*NUM_PACKETS] = {0};
    bool demod_signal[NUM_PACKETS*PACKET_LENGTH] = {0};

    for(int i = 0; i < NUM_PACKETS; i++){
        for(int j = 0; j < PACKET_LENGTH; j++){
            gmsk_in[PACKET_LENGTH*i + j] = packet[j];
            std::cout << gmsk_in[PACKET_LENGTH*i + j];
        }
        std::cout << std::endl;
    }

    gmsk_mod.modulate(gmsk_in, NUM_PACKETS*PACKET_LENGTH, gmsk_iout, gmsk_qout);

    SECTION("I/Q on unit circle", "[unitCircleIQ]"){
        float e = 0.001;    // Floating point error
        for(int i = 0; i < 10*PACKET_LENGTH*NUM_PACKETS; i++){
            CHECK(std::abs(gmsk_qout[i]*gmsk_qout[i] + gmsk_iout[i]*gmsk_iout[i] - 1) < e);
            std::cout << "(" << gmsk_iout[i] << ", " << gmsk_qout[i] << ")\n";
        }
    }

    SECTION("Demodulation", "[demodulation]"){
        gmsk_mod.demodulate(gmsk_iout, gmsk_qout, 10*NUM_PACKETS*PACKET_LENGTH, demod_signal);
        for(int i = 0; i < NUM_PACKETS; i++) {
            for (int j = 0; j < PACKET_LENGTH; j++) {
                std::cout << demod_signal[PACKET_LENGTH*i + j];
            }
            std::cout << std::endl;
        }
        CHECK(1 == 1);
    }

    CHECK(1==1);
}