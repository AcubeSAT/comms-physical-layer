#include "GMSKTranscoder.hpp"
#include "OQPSKTranscoder.hpp"

#include <iostream>
#include <string>

int main(void) {
    freopen("../test/iofiles/gmsk_input_2048.txt", "r", stdin); 
    freopen("../test/iofiles/gmsk_float_x86_output.txt", "w", stdout); 

    const int length = 2048;
    const int sps = 6;
    float inPhaseOut[sps * length] = {0};
    float quadratureOut[sps * length] = {0};

    std::string input;
    std::cin >> input; 

    etl::bitset<length> inputSignal;
    for (int i = 0; i < length; i++) {
        inputSignal.set(i, input[i] == 1); 
    }

    GMSKTranscoder<sps, length> gmskTrans(48000, 8000, false);
    gmskTrans.modulate(inputSignal, inPhaseOut, quadratureOut);

    for (int i = 0; i < sps * length; i++) {
        std::cout << inPhaseOut[i] << std::endl;
        std::cout << quadratureOut[i] << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
