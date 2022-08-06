#include "GMSKTranscoder.hpp"
#include "catch2/catch_all.hpp"
#include <fstream>
#include <iostream>
#include <string>

constexpr int PacketLength = 100;
constexpr int NumPackets = 60;
constexpr int MaxSNR = 20;

bool packet[PacketLength] = {1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0,
                                1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1};

bool gmskIn[NumPackets * PacketLength] = {0};
bool demodSignal[NumPackets * PacketLength] = {0};

// BER generated from octave prototype
double octaveBER[20] = {0.5, 0.1, 0.046109, 0.030406, 0.019704, 0.008702, 0.003401, 0.001100, 0.000500, 0.000400,
                        0.000166, 0.000166, 0.000166, 0.000166, 0.000166, 0.000166, 0.000166, 0.000166, 0.000166,
                        0.000166};
double testBER[20] = {0.401833 , 0.0943333, 0.073, 0.0571667, 0.0426667, 0.0326667, 0.0223333, 0.0148333, 0.00933333, 0.00566667,
                      0.0045, 0.0025, 0.00183333, 0.0005, 0.000166667, 0.000166667, 0.000166667, 0.000166667, 0.000166667, 0.000166667};

TEST_CASE("GMSK: SPS = 10") {
    std::fstream berFile;
    berFile.open("../test/iofiles/out_ber10.txt", std::ios::out);
    double ber[20];
    const int sps = 10;
    double gmsk_iout[sps * PacketLength * NumPackets] = {0};
    double gmsk_qout[sps * PacketLength * NumPackets] = {0};

    for (int k = 0; k < MaxSNR; k++) {
        std::string test_name = "Mod/Demod, SNR = " + std::to_string(k) + " dB";

        SECTION(test_name, "[gmskModem]") {
            int sample_frequency = 48000;
            int symbol_rate = sample_frequency / sps;
            GMSKTranscoder<sps> gmsk_mod(sample_frequency, symbol_rate, false);

            // Add NUM_PACKETS random packets to the input signal
            std::srand(std::time(nullptr));
            for (int i = 0; i < NumPackets; i++) {
                for (int j = 0; j < PacketLength; j++) {
                    gmskIn[PacketLength * i + j] = std::rand() % 2;
                }
            }

            gmsk_mod.modulate(gmskIn, NumPackets * PacketLength, gmsk_iout, gmsk_qout);

            // Writing I/Q data to a file for plotting
            std::fstream iqFile;
            iqFile.open("../test/iofiles/out_iqFileGMSK.txt", std::ios::out);
            if (iqFile.is_open()) {
                for (int i = 0; i < sps * PacketLength * NumPackets; i++) {
                    iqFile << gmsk_iout[i] << " " << gmsk_qout[i] << " ";
                }
            }

            SECTION("I/Q on unit circle", "[unitCircleIQ]") {
                for (int i = 0; i < sps * PacketLength * NumPackets; i++) {
                    CHECK(std::abs(gmsk_qout[i] * gmsk_qout[i] + gmsk_iout[i] * gmsk_iout[i]) == Catch::Approx(1));
                }
            }

            SECTION("Demodulation", "[demodulation]") {
                // Phase Shift by pi/2 (I + jQ => -Q + jI)
                double temp;
                for (int i = 0; i < sps * PacketLength * NumPackets; i++) {
                    std::swap(gmsk_qout[i], gmsk_iout[i]);
                    gmsk_iout[i] *= -1;
                }

                // Adding WGN to the received signal
                const double snr_dB = k;
                const double snr = pow(10, snr_dB / 10);
                const double variance = sample_frequency / (symbol_rate * snr);
                std::default_random_engine generator;
                std::normal_distribution<double> dist(0, 1);

                for (int i = 0; i < sps * NumPackets * PacketLength; i++) {
                    gmsk_iout[i] += sqrt(variance/2)*dist(generator);
                    gmsk_qout[i] += sqrt(variance/2)*dist(generator);
                }

                // Demodulation
                gmsk_mod.demodulate(gmsk_iout, gmsk_qout, sps * NumPackets * PacketLength, demodSignal);

                // Search for Frame location and calculate BER for the optimal position
                int minimumErrorCount = PacketLength * NumPackets;
                int minimumBitCount = PacketLength * NumPackets;
                int length = sizeof(demodSignal) / sizeof(*demodSignal);  // Length  of demod signal
                int shift;
                for (int i = 0; i < 100; ++i) {
                    int errorCount = 0;
                    for (int j = i; j < length; ++j) {
                        errorCount += demodSignal[j] ^ gmskIn[j - i];
                    }
                    if (errorCount < minimumErrorCount) {
                        minimumErrorCount = errorCount;
                        minimumBitCount = length;
                        std::cout << "Shift: " << i << " | Nerrs: " << errorCount << std::endl;
                        shift=i;
                    }

                }

                ber[k] = minimumErrorCount / static_cast<float>(minimumBitCount);
                const double ErrorCoef = 6;
                CHECK(ber[k] < ErrorCoef * testBER[k]);
                std::cout << "Bit Errors: " << minimumErrorCount << "| BER: " << ber[k] << std::endl;
            }
        }
        if(berFile.is_open()){
            berFile << ber[k] << " ";
        }
    }
}

TEST_CASE("GMSK: SPS = 6") {
    std::fstream berFile;
    berFile.open("../test/iofiles/out_ber6.txt", std::ios::out);
    double ber[20];
    const int sps = 6;
    double gmsk_iout[sps * PacketLength * NumPackets] = {0};
    double gmsk_qout[sps * PacketLength * NumPackets] = {0};

    for (int k = 0; k < MaxSNR; k++) {
        std::string test_name = "Mod/Demod, SNR = " + std::to_string(k) + " dB";

        SECTION(test_name, "[gmskModem]") {
            int sample_frequency = 48000;
            int symbol_rate = sample_frequency / sps;
            GMSKTranscoder<sps> gmsk_mod(sample_frequency, symbol_rate, false);

            // Add NUM_PACKETS random packets to the input signal
            std::srand(std::time(nullptr));
            for (int i = 0; i < NumPackets; i++) {
                for (int j = 0; j < PacketLength; j++) {
                    gmskIn[PacketLength * i + j] = std::rand() % 2;
                }
            }

            gmsk_mod.modulate(gmskIn, NumPackets * PacketLength, gmsk_iout, gmsk_qout);

            // Writing I/Q data to a file for plotting
            std::fstream iqFile;
            iqFile.open("../test/iofiles/out_iqFileGMSK.txt", std::ios::out);
            if (iqFile.is_open()) {
                for (int i = 0; i < sps * PacketLength * NumPackets; i++) {
                    iqFile << gmsk_iout[i] << " " << gmsk_qout[i] << " ";
                }
            }

            SECTION("I/Q on unit circle", "[unitCircleIQ]") {
                for (int i = 0; i < sps * PacketLength * NumPackets; i++) {
                    CHECK(std::abs(gmsk_qout[i] * gmsk_qout[i] + gmsk_iout[i] * gmsk_iout[i]) == Catch::Approx(1));
                }
            }

            SECTION("Demodulation", "[demodulation]") {
                // Phase Shift by pi/2 (I + jQ => -Q + jI)
                double temp;
                for (int i = 0; i < sps * PacketLength * NumPackets; i++) {
                    std::swap(gmsk_qout[i], gmsk_iout[i]);
                    gmsk_iout[i] *= -1;
                }

                // Adding WGN to the received signal
                const double snr_dB = k;
                const double snr = pow(10, snr_dB / 10);
                const double variance = sample_frequency / (symbol_rate * snr);
                std::default_random_engine generator;
                std::normal_distribution<double> dist(0, 1);

                for (int i = 0; i < sps * NumPackets * PacketLength; i++) {
                    gmsk_iout[i] += sqrt(variance/2)*dist(generator);
                    gmsk_qout[i] += sqrt(variance/2)*dist(generator);
                }

                // Demodulation
                gmsk_mod.demodulate(gmsk_iout, gmsk_qout, sps * NumPackets * PacketLength, demodSignal);

                // Search for Frame location and calculate BER for the optimal position
                int minimumErrorCount = PacketLength * NumPackets;
                int minimumBitCount = PacketLength * NumPackets;
                int length = sizeof(demodSignal) / sizeof(*demodSignal);  // Length  of demod signal
                int shift;
                for (int i = 0; i < 100; ++i) {
                    int errorCount = 0;
                    for (int j = i; j < length; ++j) {
                        errorCount += demodSignal[j] ^ gmskIn[j - i];
                    }
                    if (errorCount < minimumErrorCount) {
                        minimumErrorCount = errorCount;
                        minimumBitCount = length;
                        std::cout << "Shift: " << i << " | Nerrs: " << errorCount << std::endl;
                        shift=i;
                    }

                }

                ber[k] = minimumErrorCount / static_cast<float>(minimumBitCount);
                const double ErrorCoef = 6;
                CHECK(ber[k] < ErrorCoef * testBER[k]);
                std::cout << "Bit Errors: " << minimumErrorCount << "| BER: " << ber[k] << std::endl;
            }
        }
        if(berFile.is_open()){
            berFile << ber[k] << " ";
        }
    }
}
