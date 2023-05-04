#include "GMSKTranscoder.hpp"
#include "OQPSKTranscoder.hpp"

#include <iostream>
#include <fstream> 
#include <string>
#include <cstdint>

int main(void) {
    std::ifstream file("../test/iofiles/gmsk_input.txt");
    std::ofstream iqFile("../test/iofiles/gmsk_float_x86_output.txt"); 

    if (!file.is_open()) {
        std::cout << "Cannot open input" << std::endl; 
        return -1; 
    }

    if (!iqFile.is_open()) {
        std::cout << "Cannot open output" << std::endl; 
        return -1; 
    }

    const uint16_t length = 2048;
    const uint8_t sps = 6;
    float inPhaseOut[sps * length] = {0};
    float quadratureOut[sps * length] = {0};

    std::string input;
    file >> input; 

    etl::bitset<length> inputSignal;
    etl::bitset<length> outputSignal;
    for (int i = 0; i < length; i++) {
        inputSignal.set(i, input[i] == '1');
    }


    GMSKTranscoder<sps, length> gmskTrans(48000, 8000, false);
    gmskTrans.modulate(inputSignal, inPhaseOut, quadratureOut);

    for (int i = 0; i < sps * length; i++) {
        iqFile << inPhaseOut[i] << std::endl;
        iqFile << quadratureOut[i] << std::endl;
        iqFile << std::endl;
    }

    gmskTrans.demodulate(inPhaseOut, quadratureOut, length, outputSignal);
    int optimalErrors = length;
    int shift = 0;
    for (int i = 0; i < length; i++) {
        int errors = 0;
        for (int j = i; j < inputSignal.size(); j++) {
            errors += outputSignal[j] ^ inputSignal[j - i];
        }
        if (errors < optimalErrors) {
            optimalErrors = errors;
            shift = i;
        }
    }

    printf("%d %d\n", optimalErrors, shift);
    file.close(); 
    iqFile.close(); 
    return 0;
}
