#include <gmsk.h>
#include <iostream>
#include <cstring>
#include "filter.h"

double gmsk_mod_coeff[] = {    // Gaussian filter taps for BTs = 0.25
        0.00012196358875371516, 0.0003575949085643515, 0.0007969101279741153, 0.00158733366697561,
        0.0029597436514450237, 0.005259322046185844, 0.008977695411886089, 0.01477996823086869,
        0.023517398207332008, 0.03621464785828721, 0.053899076432571746, 0.07776153780287132,
        0.10879461496369913, 0.1476700841449201, 0.19454187480732799, 0.24886796809732914,
        0.3092885660007596, 0.373594855889678, 0.4388112183660269, 0.5013958122581244,
        0.5575432125478983, 0.6035536825656891, 0.6362202614545822, 0.6531800404191017,
        0.6531800404191017, 0.6362202614545822, 0.6035536825656891, 0.5575432125478983,
        0.5013958122581244, 0.4388112183660269, 0.373594855889678, 0.3092885660007596,
        0.24886796809732914, 0.19454187480732799, 0.1476700841449201, 0.10879461496369913,
        0.07776153780287132, 0.053899076432571746, 0.03621464785828721, 0.023578319309308426,
        0.014840889332845109, 0.009038616513862507, 0.0053202431481622625, 0.0030206647534214426,
        0.0016482547689520288,0.0008578312299505342, 0.00041851601054077037, 0.00018288469073013403,
        6.092110197641887e-05};

double gmsk_demod_coeff[] = {
        -0.000153959924563, 0.000000000000000, 0.000167227768379, 0.000341615513437,
        0.000513334449696, 0.000667493753523, 0.000783901543032, 0.000838293462576,
        0.000805143268199, 0.000661865814384, 0.000393913058926, -0.000000000000000,
        -0.000503471198655, -0.001079755887508, -0.001671728086040, -0.002205032425392,
        -0.002594597675000, -0.002754194565297, -0.002608210441859, -0.002104352817854,
        -0.001225654870420, 0.000000000000000, 0.001494548041184, 0.003130012785731,
        0.004735238379172, 0.006109242742194, 0.007040527007323, 0.007330850462455,
        0.006821247169795, 0.005417521811131, 0.003112202160626, -0.000000000000000,
        -0.003715739376345, -0.007727358782391, -0.011638713107503, -0.014992029537478,
        -0.017304097563429, -0.018108937286588, -0.017003180218569, -0.013689829477969,
        -0.008015928769710, 0.000000000000000, 0.010154104792614, 0.022059114281395,
        0.035162729807337, 0.048781621388364, 0.062148583345584, 0.074469032280094,
        0.084982001723750, 0.093020219991183, 0.098063819576269, 0.099782731268437,
        0.098063819576269, 0.093020219991183, 0.084982001723750, 0.074469032280094,
        0.062148583345584, 0.048781621388364, 0.035162729807337, 0.022059114281395,
        0.010154104792614, 0.000000000000000, -0.008015928769710, -0.013689829477969,
        -0.017003180218569, -0.018108937286588, -0.017304097563429, -0.014992029537478,
        -0.011638713107503, -0.007727358782391, -0.003715739376345, -0.000000000000000,
        0.003112202160626, 0.005417521811131, 0.006821247169795, 0.007330850462455,
        0.007040527007323, 0.006109242742194, 0.004735238379172, 0.003130012785731,
        0.001494548041184, 0.000000000000000, -0.001225654870420, -0.002104352817854,
        -0.002608210441859, -0.002754194565297, -0.002594597675000, -0.002205032425392,
        -0.001671728086040, -0.001079755887508, -0.000503471198655, -0.000000000000000,
        0.000393913058926, 0.000661865814384, 0.000805143268199, 0.000838293462576,
        0.000783901543032, 0.000667493753523, 0.000513334449696, 0.000341615513437,
        0.000167227768379, 0.000000000000000, -0.000153959924563
};

void GMSKTranscoder::modulate(int8_t *signal, uint16_t signal_length, double *in_phase_signal,
                              double *quadrature_signal) {

    uint16_t samples_n = samples_per_symbol * signal_length;

    internal_buffer_quadrature[0] = (signal[0]+1)%2;
    for (uint16_t i = 0; i < signal_length-1; i++){
        internal_buffer_quadrature[i+1] = (signal[i+1]+signal[i] + i+1)%2;
    }

    // NRZ encoding
    for (uint16_t i = 0; i < signal_length; i++) {
        // TODO: HAL implementation
        for (uint16_t j = 0; j < samples_per_symbol; j++) {
            internal_buffer_in_phase[i * samples_per_symbol + j] = -1 + 2 * (internal_buffer_quadrature[i]);
        }
    }

    // Precoding
    uint16_t s = 41;

    // Filter matching
    filter_fir(gmsk_mod_coeff, s, internal_buffer_in_phase, samples_n, internal_buffer_quadrature);

    // FM Modulator
    fm_transcoder.modulate(internal_buffer_quadrature, samples_n, in_phase_signal, quadrature_signal);
}

void GMSKTranscoder::demodulate(double *input_in_phase_signal, double *input_quadrature_signal, uint16_t signal_length,
                                bool *signal) {

    uint16_t symbols_n = floor(signal_length / samples_per_symbol);
    float frequency_deviation_component = 2 * M_PI * max_deviation / sampling_frequency;
    double rx_int[2 << 12] = {0};

    filter_fir(gmsk_mod_coeff, 29, input_in_phase_signal, signal_length, internal_buffer_in_phase);
    filter_fir(gmsk_mod_coeff, 29, input_quadrature_signal, signal_length, internal_buffer_quadrature);

    integrate(internal_buffer_in_phase, signal_length, 2*samples_per_symbol, input_in_phase_signal);
    integrate(internal_buffer_quadrature, signal_length, 2*samples_per_symbol, input_quadrature_signal);

    // Add Wiener Filter for equalisation
    filter_fir(delayed_taps, 6*samples_per_symbol, input_in_phase_signal, signal_length, internal_buffer_in_phase);
    filter_fir(delayed_taps, 6*samples_per_symbol, input_quadrature_signal, signal_length, internal_buffer_quadrature);

    // Implement PLL
    double dco = 0;
    double lower = 0;
    double loop_filt = 0;
    double phase_error = 0;
    double timing_clock_phase = 0;
    uint16_t timing_window = 10*samples_per_symbol; // TODO: 200*samples_per_symbol;
    double timing_angle = 0;
    double* timing_angle_log = input_in_phase_signal;
    double* toff = input_quadrature_signal;

    double temp;

    for (uint16_t i; i < signal_length; i++){
        if (i > 1 && (i+1) % timing_window == 0){
            double i_clock_phase = 0;
            double q_clock_phase = 0;
            for (uint16_t j = i - timing_window + 1; j <= i; j++){
                i_clock_phase += fabs(internal_buffer_in_phase[j])*cos((j+1)*2*M_PI*(symbol_rate/2.0)/sampling_frequency);
                q_clock_phase -= fabs(internal_buffer_in_phase[j])*sin((j+1)*2*M_PI*(symbol_rate/2.0)/sampling_frequency);
                }
            timing_angle = atan2(q_clock_phase, i_clock_phase);
            timing_clock_phase = timing_angle;
        } else{
            timing_clock_phase += 2.0*M_PI/(2*samples_per_symbol);
        }
        timing_angle_log[i] = timing_angle;

        temp = internal_buffer_in_phase[i]*cos(dco) + internal_buffer_quadrature[i]*sin(dco);
        internal_buffer_quadrature[i] = -internal_buffer_in_phase[i]*sin(dco) + cos(dco)*internal_buffer_quadrature[i];
        internal_buffer_in_phase[i] = temp;
        phase_error = (-1+2*signs(internal_buffer_in_phase[i]*internal_buffer_quadrature[i]))*cos(timing_clock_phase);
        lower = pll_params.G2*phase_error + lower;
        loop_filt = pll_params.G1*phase_error + lower;
        dco = dco + loop_filt;
    }

    toff[0] = floor(timing_angle_log[0] + 0.5);
    for (uint16_t i = 1; i < signal_length; i++){
        // The difference of of consecutive angles must be smaller than π
        toff[i] = floor(timing_angle_log[i - 1] + M_PI * floor((timing_angle_log[i] - timing_angle_log[i - 1]) / M_PI) + 0.5);
    }
    
    uint16_t k = 0;
    for (uint16_t i = 2*samples_per_symbol; i < signal_length; i += 2*samples_per_symbol){
        if ((i - toff[i] + samples_per_symbol) < signal_length) {
            internal_buffer_in_phase[k] = internal_buffer_in_phase[i - static_cast<uint16_t>(toff[i])];
            internal_buffer_quadrature[k] = internal_buffer_quadrature[i - static_cast<uint16_t>(toff[i]) + samples_per_symbol];
        }
        k += 1;
    }

    for (uint16_t i = 0; i < symbols_n/2; i++){
        signal[2*i] = internal_buffer_in_phase[i] < 0;
        signal[2*i+1] = internal_buffer_quadrature[i] < 0;
    }

}
