#define CATCH_CONFIG_MAIN

#include "catch2/catch_all.hpp"
#include "Synchronizer.hpp"

#include <iostream>
#include <fstream>

TEST_CASE("Test FFT"){
    SECTION("FFT of Square Wave"){
        double numpyFFT[32] = {0, 0.835493, 0.941979, 0.203869, 0.765367, 1.1121,
                               0.460249, 0.71871, 1.41421, 0.87575, 0.688812, 2.08059,
                               1.84776, 0.672066, 4.73565, 8.4829, 10, 8.4829, 4.73565,
                               0.672066, 1.84776, 2.08059, 0.688812, 0.87575, 1.41421,
                               0.71871, 0.460249, 1.1121, 0.765367, 0.203869, 0.941979, 0.835493
        };

        uint8_t signalLength = 32;
        ComplexArray sqwave(signalLength);
        Complex a = {0, 0};
        Complex b = {1, 0};
        std::fill(sqwave.begin(), sqwave.begin() + 10, a);
        std::fill(sqwave.begin() + 10, sqwave.begin() + 20, b);
        std::fill(sqwave.begin() + 20, sqwave.begin() + 32, a);
        fft(sqwave);
        fftshift(sqwave, signalLength);
        for (int i = 0; i < signalLength; ++i) {
            CHECK(std::abs(sqwave[i]) == Catch::Approx(numpyFFT[i]));
        }

    }
}

TEST_CASE("Synchronization Test: ASM exists"){

    constexpr uint8_t samplesPerSymbol = 10;
    constexpr uint8_t messageLength = 0;
    constexpr uint8_t excessBits = 32;
    uint16_t sampleFrequency = 48000;
    uint16_t symbolRate = sampleFrequency / samplesPerSymbol;
    constexpr uint16_t inputLength = asmLength + messageLength + tailLength + excessBits;

    std::srand(std::time(nullptr));
    bool input[inputLength] = {0};

    double cltuInPhase[inputLength * samplesPerSymbol] = {0};
    double cltuQuadrature[inputLength * samplesPerSymbol] = {0};
    const uint16_t correlationLength = (samplesPerSymbol * (inputLength - asmLength)) / (samplesPerSymbol / 4);
    double correlationSignal[correlationLength] = {0};

    auto synchronizer = Synchronizer<samplesPerSymbol, inputLength>(sampleFrequency, symbolRate);

    std::fstream acqFile;
    acqFile.open("../test/iofiles/acq.txt", std::ios::out);

    // Input signal initialization
    for (int i = 0; i < 32; ++i) {
        input[i] = std::rand() % 2;
    }
    for (int i = 0; i < asmLength; ++i) {
        input[i + excessBits] = synchronizer.startSequence[i];
    }
    for (int i = 0; i < messageLength; ++i) {
        input[i + excessBits + asmLength] = std::rand() % 2;
    }
    for (int i = 0; i < tailLength; ++i) {
        input[i + excessBits + asmLength + messageLength] = synchronizer.tailSequence[i];
    }
    SECTION("ACQ Calculation"){
        // CLTU Packet
        synchronizer.gmsk.modulate(input, inputLength,
                                   cltuInPhase, cltuQuadrature);
        synchronizer.computeCorrelation(cltuInPhase, cltuQuadrature,
                                        inputLength * samplesPerSymbol);

        // Calculate correlation signal from acqBuffer (Max sample from each row)
        for (int i = 0; i < (samplesPerSymbol * (inputLength - asmLength)) / (samplesPerSymbol / 4); ++i) {
            correlationSignal[i] = synchronizer.getAcqBuffer(i, 0);
            for (int j = 0; j < MaxPowerOf2; ++j) {
                if (synchronizer.getAcqBuffer(i, j) > correlationSignal[i]){
                    correlationSignal[i] = synchronizer.getAcqBuffer(i, j);
                }
            }
        }
        for (int i = 0; i < (samplesPerSymbol * (inputLength - asmLength)) / (samplesPerSymbol / 4); ++i) {
            for (int j = 0; j < MaxPowerOf2; ++j) {
                acqFile << synchronizer.getAcqBuffer(i, j) << " ";
            }
            acqFile << std::endl;
        }
        // Calculate the mean and variance of the correlation signal
        double mean;
        double meanSum = 0;
        double variance;
        double varianceSum = 0;
        for (int i = 0; i < correlationLength; ++i) {
            meanSum += correlationSignal[i];
        }
        mean = meanSum / correlationLength;

        for (int i = 0; i < correlationLength; ++i) {
            varianceSum += (correlationSignal[i] - mean) * (correlationSignal[i] - mean);
        }
        variance = varianceSum / (correlationLength - 1);

        double sigma = std::sqrt(variance);
        std::cout << "Mean : " << mean << std::endl;
        std::cout << "Variance : " << variance << std::endl;
        std::cout << "Standard Deviation : " << sigma << std::endl;

        // Check if a spike exists in the correlation signal
        bool spikeExists = false;
        for (int i = 0; i < correlationLength; ++i) {
            if (correlationSignal[i] > 8 * sigma){
                /**
                 * The spike sample will be at sample: (bitPosition * samplesPerSymbol) / (samplesPerSymbol / 4)
                 * If ASM starts at bit position 32:
                 * For SPS = 10: Spike at sample 160
                 * For SPS = 6: Spike at sample 196
                 * etc.
                 */
                spikeExists = true;
                std::cout << "Spike: Value = " << correlationSignal[i] << " | at sample " << i << std::endl;
                CHECK(std::abs(i - (excessBits * samplesPerSymbol) / (samplesPerSymbol / 4)) <= 3);
            }
        }

        CHECK(spikeExists);

        acqFile.close();
    }
}

TEST_CASE("Synchronization Test: Random Signal"){

    constexpr uint8_t samplesPerSymbol = 10;
    constexpr uint8_t messageLength = 0;
    constexpr uint8_t excessBits = 32;
    uint16_t sampleFrequency = 48000;
    uint16_t symbolRate = sampleFrequency / samplesPerSymbol;
    constexpr uint16_t inputLength = asmLength + messageLength + tailLength + excessBits;

    std::srand(std::time(nullptr));
    bool input[inputLength] = {0};

    double cltuInPhase[inputLength * samplesPerSymbol] = {0};
    double cltuQuadrature[inputLength * samplesPerSymbol] = {0};
    const uint16_t correlationLength = (samplesPerSymbol * (inputLength - asmLength)) / (samplesPerSymbol / 4);
    double correlationSignal[correlationLength] = {0};

    auto synchronizer = Synchronizer<samplesPerSymbol, inputLength>(sampleFrequency, symbolRate);

    std::fstream acqFile;
    acqFile.open("../test/iofiles/acqRandom.txt", std::ios::out);

    // Random signal initialization
    for (int i = 0; i < inputLength; ++i) {
        input[i] = std::rand() % 2;
    }

    SECTION("ACQ Calculation"){
        // CLTU Packet
        synchronizer.gmsk.modulate(input, inputLength,
                                   cltuInPhase, cltuQuadrature);
        synchronizer.computeCorrelation(cltuInPhase, cltuQuadrature,
                                        inputLength * samplesPerSymbol);

        // Calculate correlation signal from acqBuffer (Max sample from each row)
        for (int i = 0; i < (samplesPerSymbol * (inputLength - asmLength)) / (samplesPerSymbol / 4); ++i) {
            correlationSignal[i] = synchronizer.getAcqBuffer(i, 0);
            for (int j = 0; j < MaxPowerOf2; ++j) {
                if (synchronizer.getAcqBuffer(i, j) > correlationSignal[i]){
                    correlationSignal[i] = synchronizer.getAcqBuffer(i, j);
                }
            }
        }
        for (int i = 0; i < (samplesPerSymbol * (inputLength - asmLength)) / (samplesPerSymbol / 4); ++i) {
            for (int j = 0; j < MaxPowerOf2; ++j) {
                acqFile << synchronizer.getAcqBuffer(i, j) << " ";
            }
            acqFile << std::endl;
        }
        // Calculate the mean and variance of the correlation signal
        double mean;
        double meanSum = 0;
        double variance;
        double varianceSum = 0;
        for (int i = 0; i < correlationLength; ++i) {
            meanSum += correlationSignal[i];
        }
        mean = meanSum / correlationLength;

        for (int i = 0; i < correlationLength; ++i) {
            varianceSum += (correlationSignal[i] - mean) * (correlationSignal[i] - mean);
        }
        variance = varianceSum / (correlationLength - 1);

        double sigma = std::sqrt(variance);
        std::cout << "Mean : " << mean << std::endl;
        std::cout << "Variance : " << variance << std::endl;
        std::cout << "Standard Deviation : " << sigma << std::endl;

        // Check if a spike exists in the correlation signal
        bool spikeExists = false;
        for (int i = 0; i < correlationLength; ++i) {
            if (correlationSignal[i] > 9 * sigma){
                spikeExists = true;
                std::cout << "Spike: Value = " << correlationSignal[i] << " | at sample " << i << std::endl;
            }
        }

        CHECK(!spikeExists);

        acqFile.close();
    }
}
