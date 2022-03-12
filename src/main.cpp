#include "gmsk.h"
#include <iostream>

int main() {
    double out_signal[10000] = {0};
    double i_signal[10000] = {0};
    double q_signal[10000] = {0};
    double in_signal[] = {-6.45590600723470e-14, -1.10162644135736e-12, -1.55499780047108e-11, -1.90128646305855e-10,
                          -2.01959995160422e-09, -1.86468940229638e-08, -1.49709591893155e-07, -1.04568931053421e-06,
                          -6.35794297383698e-06, -3.36741867754016e-05, -0.000155495900666193, -0.000626679298013012,
                          -0.00220726044924373, -0.00680564353391649, -0.0184082341838734, -0.0437984278555478,
                          -0.0919862440607759, -0.171302895512982, -0.284524565532716, -0.424663466847524};
    int8_t gmsk_in_signal[] = {0, 0, 1, 0, 1, 0, 1, 1, 1, 0};
    bool output[1000] = {0};
    //FMTranscoder fm_modulate(48000, 24000, 0, 1200, 1, 1);
    GMSKTranscoder gmsk_modulate(48000, 4800);
    gmsk_modulate.modulate(gmsk_in_signal, 10, i_signal, q_signal);

    double temp;
    // Phase rotation of modulated signal
    for(uint8_t  i=0; i<100; i++) {
        temp = i_signal[i] * cos(M_PI / 2) + q_signal[i] * sin(M_PI / 2);
        q_signal[i] = -i_signal[i] * sin(M_PI / 2) + cos(M_PI / 2) * q_signal[i];
        i_signal[i] = temp;
    }
    gmsk_modulate.demodulate(i_signal, q_signal, 100, output);
    for(int i =0; i<10; i++)
    {
        std::cout<<static_cast<uint16_t>(gmsk_in_signal[i])<<" ";
    }
    std::cout<<"\n";

    for(int i =0; i<10; i++)
    {
        std::cout<<output[i]<<" ";
    }
    //fm_modulate.modulate(in_signal, 20, i_signal, q_signal);
    return 0;
}
