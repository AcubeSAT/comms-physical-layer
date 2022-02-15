#include <gmsk.h>
#include <iostream>
#include "filter.h"

double gmsk_mod_coeff[] = {
        6.455906007234699e-014, 1.037067381285011e-012, 1.444835156335346e-011,
        1.745786683011439e-010, 1.829471305298363e-009, 1.662729407135958e-008,
        1.310626978701910e-007, 8.959797186410516e-007, 5.312253663302771e-006,
        2.731624380156465e-005, 1.218217140199093e-004, 4.711833994209542e-004,
        1.580581180127418e-003, 4.598383433830095e-003, 1.160259430889949e-002,
        2.539022692626253e-002, 4.818807833062393e-002, 7.931844341164322e-002,
        1.132322945270602e-001, 1.401935338024111e-001, 1.505383695578516e-001,
        1.401935338024111e-001, 1.132322945270601e-001, 7.931844341164328e-002,
        4.818807833062393e-002, 2.539022692626253e-002, 1.160259430889949e-002,
        4.598383433830090e-003, 1.580581180127420e-003, 4.711833994209542e-004,
        1.218217140199093e-004, 2.731624380156465e-005, 5.312253663302753e-006,
        8.959797186410563e-007, 1.310626978701910e-007, 1.662729407135958e-008,
        1.829471305298363e-009, 1.745786683011426e-010, 1.444835156335356e-011,
        1.037067381285011e-012, 6.455906007234699e-014};

double gmsk_demod_coef[] = {
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

void GMSKTranscoder::modulate(const double *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal){
    uint16_t samples_n = samples_per_symbol * signal_length;

    for (uint16_t i = 0; i < signal_length; i++){
        // TODO: HAL implementation
        for (uint16_t j = 0; j < samples_per_symbol; j++){
            internal_buffer[i*samples_per_symbol + j] = -1 + 2*(signal[i]);
        }
    }
    uint16_t s = 41;

    void filter_fir(const double* filter_taps, uint16_t number_of_taps, const double* input_signal, uint16_t size, double* output_signal);

    filter_fir(gmsk_mod_coeff, s, internal_buffer, samples_n, internal_buffer2);
    fm_transcoder.modulate(internal_buffer2, samples_n, in_phase_signal, quadrature_signal);

    for (int i = 0; i < samples_n; i++){
        std::cout << in_phase_signal[i] <<" ";
    }
    std::cout << std::endl;
    for (int i = 0; i < samples_n; i++){
        std::cout << quadrature_signal[i] <<" ";
    }

}
