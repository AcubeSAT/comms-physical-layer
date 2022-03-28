#include "main.h"
#include "oqpsk.hpp"
extern "C" void main_cpp(float32_t* i_signal, float32_t* q_signal) {

    uint8_t gmsk_in_signal[] = {0, 0, 0, 1, 1, 0, 1, 1};

    OQPSKTranscoder oqpsk_modulate(10);
    oqpsk_modulate.modulate(gmsk_in_signal, 8, i_signal, q_signal);
};