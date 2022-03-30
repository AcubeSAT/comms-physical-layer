#include "main.h"
#include "oqpsk.hpp"
extern "C" uint32_t main_cpp(float32_t* i_signal, float32_t* q_signal) {

    uint8_t gmsk_in_signal[1000];
    for(int i =0; i<1000; i++){
        if(i%2==0)
        gmsk_in_signal[i]=0;
        else
            gmsk_in_signal[i]=1;
    }

    OQPSKTranscoder oqpsk_modulate(10);
    uint32_t before_millis = HAL_GetTick();
    for(int i =0; i<200; i++)
    oqpsk_modulate.modulate(gmsk_in_signal, 1000, i_signal, q_signal);

    uint32_t modulation_time = HAL_GetTick() - before_millis;

    return modulation_time;
};