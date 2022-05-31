import numpy
import matplotlib.pyplot as plt
from gnuradio import filter

sps = 10
ntaps = 40

ccsdstaps = filter.firdes.gaussian(1, sps, 0.25, ntaps)    # The taps that are going to be used
ccsdstaps2 = filter.firdes.gaussian(1, sps, 0.5, ntaps)    # These are just for comparison (higher BTs)

sqwave = (1,) * sps  # Convolution with window function as the standard suggests
ccsdstaps_conv = numpy.convolve(numpy.array(ccsdstaps), numpy.array(sqwave))
ccsdstaps2_conv = numpy.convolve(numpy.array(ccsdstaps2), numpy.array(sqwave))

print(list(ccsdstaps_conv))  # The taps that are going to be used in gmsk.cpp

plt.plot(ccsdstaps, 'g^', ccsdstaps2, 'r^')
plt.plot(ccsdstaps_conv, 'go', ccsdstaps2_conv, 'ro')
plt.show()
