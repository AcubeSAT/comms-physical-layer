import numpy
import matplotlib.pyplot as plt
from gnuradio import filter

# The following script calculates the Gaussian filter taps that are going to be used in
# GMSKTranscoder and that are defined in GMSKFilterTaps.hpp
#
# The taps are calculated with 2 distinct parameters:
# BTs (Bandwidth * Symbol Period) and SPS (Samples per Symbol)
#
# hardTaps: the gaussian filter taps provided by Rowetel's octave prototypes (https://github.com/drowe67/codec2)

sps = 10
ntaps = 41

ccsdstaps = filter.firdes.gaussian(1, sps, 0.25, ntaps)    # The taps that are going to be used
ccsdstaps2 = filter.firdes.gaussian(1, sps, 0.5, ntaps)    # These are just for comparison (higher BTs)
hardTaps = [6.455906007234699e-014, 1.037067381285011e-012, 1.444835156335346e-011,
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
            1.037067381285011e-012, 6.455906007234699e-014]

sqwave = (1,) * sps  # Convolution with window function as the standard suggests
ccsdstaps_conv = numpy.convolve(numpy.array(ccsdstaps), numpy.array(sqwave))
ccsdstaps2_conv = numpy.convolve(numpy.array(ccsdstaps2), numpy.array(sqwave))

plt.plot(hardTaps, 'bo', filter.firdes.gaussian(1, sps, 0.5, 42), 'r-') # They are the same
plt.plot(filter.firdes.gaussian(1, sps, 0.25, 42))
plt.plot(filter.firdes.gaussian(1, 4, 0.25, 42), 'g-')
plt.plot(filter.firdes.gaussian(1, 6, 0.25, 42), 'm-')
plt.show()

print(filter.firdes.gaussian(1, sps, 0.25, 42))
print(filter.firdes.gaussian(1, 4, 0.25, 42))
print(filter.firdes.gaussian(1, 6, 0.25, 42))
