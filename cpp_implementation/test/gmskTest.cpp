#include "catch2/catch_all.hpp"
#include "gmsk.h"
#include <iostream>
#include <fstream>
#include <string>

#define PACKET_LENGTH 100
#define NUM_PACKETS 60
#define PACKETS_TO_IGNORE 2

int8_t packet[PACKET_LENGTH] = {1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0,
                                1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1};

int8_t gmsk_in[NUM_PACKETS*PACKET_LENGTH] = {0};
double gmsk_iout[10*PACKET_LENGTH*NUM_PACKETS] = {0};  // Max samples per symbol tested = 10
double gmsk_qout[10*PACKET_LENGTH*NUM_PACKETS] = {0};
bool demod_signal[NUM_PACKETS*PACKET_LENGTH] = {0};

TEST_CASE("SPS = 10") {
    std::fstream berFile;
    berFile.open("../tests/iofiles/ber.txt", std::ios::out);
    double ber[20];

    for (int k = 0; k < 20; k++) {
        std::string test_name = "GMSK mod/demod, SNR = " + std::to_string(k) + " dB";

        SECTION(test_name, "[gmskModem]") {

            int sps = 10;
            int sample_frequency = 48000;
            int symbol_rate = sample_frequency / sps;
            GMSKTranscoder gmsk_mod(sample_frequency, symbol_rate, false);

            // Add NUM_PACKETS random packets to the input signal
            std::srand(std::time(nullptr));
            for (int i = 0; i < NUM_PACKETS; i++) {
                for (int j = 0; j < PACKET_LENGTH; j++) {
                    gmsk_in[PACKET_LENGTH * i + j] = std::rand() % 2;
                }
            }

            gmsk_mod.modulate(gmsk_in, NUM_PACKETS * PACKET_LENGTH, gmsk_iout, gmsk_qout);

            // Writing I/Q data to a file for plotting
            std::fstream iqFile;
            iqFile.open("../tests/iofiles/iqFileGMSK11.txt", std::ios::out);
            if (iqFile.is_open()) {
                for (int i = 0; i < sps * PACKET_LENGTH * NUM_PACKETS; i++) {
                    iqFile << gmsk_iout[i] << " " << gmsk_qout[i] << " ";
                }
            }

            SECTION("I/Q on unit circle", "[unitCircleIQ]") {
                float e = 0.001;    // Floating point error
                for (int i = 0; i < sps * PACKET_LENGTH * NUM_PACKETS; i++) {
                    CHECK(std::abs(gmsk_qout[i] * gmsk_qout[i] + gmsk_iout[i] * gmsk_iout[i] - 1) < e);
                }
            }

            SECTION("Demodulation", "[demodulation]") {
                // Phase Shift by pi/2 (I + jQ => -Q + jI)
                double temp;
                for (int i = 0; i < sps * PACKET_LENGTH * NUM_PACKETS; i++) {
                    temp = gmsk_qout[i];
                    gmsk_qout[i] = gmsk_iout[i];
                    gmsk_iout[i] = -temp;
                }

                // Adding WGN to the received signal
                const double snr_dB = k;
                const double snr = pow(10, snr_dB / 10);
                const double variance = sample_frequency / (symbol_rate * snr);
                std::default_random_engine generator;
                std::normal_distribution<double> dist(0, 1);

                for (int i = 0; i < sps * NUM_PACKETS * PACKET_LENGTH; i++) {
                    gmsk_iout[i] += sqrt(variance/2)*dist(generator);
                    gmsk_qout[i] += sqrt(variance/2)*dist(generator);
                }

                // Demodulation
                gmsk_mod.demodulate(gmsk_iout, gmsk_qout, sps * NUM_PACKETS * PACKET_LENGTH, demod_signal);

                // Search for Frame location and calculate BER for the optimal position
                int Nerrs_min = PACKET_LENGTH * NUM_PACKETS;
                int Nbits_min = PACKET_LENGTH * NUM_PACKETS;
                int length = sizeof(demod_signal) / sizeof(*demod_signal);  // Length  of demod signal
                int shift;
                for (int i = 0; i < 100; ++i) {
                    int Nerrs = 0;
                    for (int j = i; j < length; ++j) {
                        Nerrs += demod_signal[j] ^ gmsk_in[j - i];
                    }
                    if (Nerrs < Nerrs_min) {
                        Nerrs_min = Nerrs;
                        Nbits_min = length;
                        std::cout << "Shift: " << i << " |Nerrs: " << Nerrs << std::endl;
                        shift=i;
                    }

                }

                ber[k] = (float) Nerrs_min / (float) Nbits_min;
                CHECK(ber[k] < 0.5);
                std::cout << "Bit Errors: " << Nerrs_min << "| BER: " << ber[k] << std::endl;
            }
        }
        if(berFile.is_open()){
            berFile << ber[k] << " ";
        }
    }
}
