#include "Channel.h"
#include "GMSKTranscoder.hpp"
#include "catch2/catch_all.hpp"
#include <fstream>
#include <iostream>
#include <string>

constexpr uint16_t PacketLength = 100;
constexpr uint16_t NumPackets = 60;
constexpr uint8_t MaxSNR = 20;
etl::bitset<NumPackets *PacketLength> gmskInput =
    etl::bitset<NumPackets * PacketLength>();

const uint8_t samplesPerSymbol = 10;
double gmskIout[samplesPerSymbol * PacketLength * NumPackets] = {0};
double gmskQout[samplesPerSymbol * PacketLength * NumPackets] = {0};



Channel channel;

TEST_CASE("TEST EFFECTS ON GMSK SIGNAL") {
    double gmskIoutTemp[samplesPerSymbol * PacketLength * NumPackets];
    double gmskQoutTemp[samplesPerSymbol * PacketLength * NumPackets];


    int sampleFrequency = 48000;
    int symbolRate = sampleFrequency / samplesPerSymbol;
    GMSKTranscoder<samplesPerSymbol, NumPackets * PacketLength> gmskMod(
        sampleFrequency, symbolRate, false);

    // Add NUM_PACKETS random packets to the input signal
    std::srand(std::time(nullptr));
    for (int i = 0; i < NumPackets; i++) {
        for (int j = 0; j < PacketLength; j++) {
            gmskInput.set(PacketLength * i + j, std::rand() % 2);
        }
    }

    gmskMod.modulate(gmskInput, gmskIout, gmskQout);

    for(int j = 0; j < samplesPerSymbol * PacketLength * NumPackets; j++){
        gmskIoutTemp[j] = gmskIout[j];
        gmskQoutTemp[j] = gmskQout[j];
    }

    // create a file with i/q data
    std::fstream channelFile;
    channelFile.open("../test/iofiles/gmskSignal.txt", std::ios::out);
    if (channelFile.is_open()) {
        for (int i = 0; i < samplesPerSymbol * PacketLength * NumPackets;
             i++) {
            channelFile << gmskIout[i] << " " << gmskQout[i] << " ";
        }
    }

    //AGWN TO GMSK SIGNAL
    double snrDB = 10;
    channel.addWGN(gmskIout, gmskQout, snrDB, symbolRate, samplesPerSymbol,
                   NumPackets, PacketLength);

    // create a file with i/q data
    std::fstream addWGNFile;
    addWGNFile.open("../test/iofiles/wgnSignal.txt", std::ios::out);
    if (addWGNFile.is_open()) {
        for (int i = 0; i < samplesPerSymbol * PacketLength * NumPackets;
             i++) {
            addWGNFile << gmskIout[i] << " " << gmskQout[i] << " ";
        }
    }

    for(int j = 0; j < samplesPerSymbol * PacketLength * NumPackets; j++){
        gmskIout[j] = gmskIoutTemp[j];
        gmskQout[j] = gmskQoutTemp[j];
    }


    // PHASE SHIFT GMSK SIGNAL
    channel.shiftPhase(gmskIout, gmskQout, samplesPerSymbol, NumPackets,
                       PacketLength, M_PI);

    std::fstream phaseShiftFile;
    phaseShiftFile.open("../test/iofiles/phaseShiftedSignal.txt",
                        std::ios::out);
    if (phaseShiftFile.is_open()) {
        for (int i = 0; i < samplesPerSymbol * PacketLength * NumPackets;
             i++) {
            phaseShiftFile << gmskIout[i] << " " << gmskQout[i] << " ";
        }
    }

    //FREQUENCY SHIFT GMSK SIGNAL
    double freqShift = 1000;
    channel.shiftFrequency(gmskIout, gmskQout, samplesPerSymbol, NumPackets,
                           PacketLength, freqShift, 2000);

    // create a file with i/q data
    std::fstream freqShiftFile;
    freqShiftFile.open("../test/iofiles/freqShiftedSignal.txt",
                       std::ios::out);
    if (freqShiftFile.is_open()) {
        for (int i = 0; i < samplesPerSymbol * PacketLength * NumPackets;
             i++) {
            freqShiftFile << gmskIout[i] << " " << gmskQout[i] << " ";
        }
    }
}

