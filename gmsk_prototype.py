#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import scipy.io.wavfile
import scipy.signal

def get_start_sequence():
        a = [0xED, 0x48]
        return np.unpackbits(np.array(a, dtype='uint8'))

def get_tail_sequence():
    a = [0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0x79]
    return np.unpackbits(np.array(a, dtype='uint8'))


def gaussian_taps(bt, samples_per_symbol, ntaps):
    taps = np.exp(-2*np.pi**2*bt**2*(np.arange(ntaps) - ntaps/2)**2/(np.log(2)*samples_per_symbol**2))
    return taps / np.sum(taps)

# function for generating the start sequence of the CLTU
def generate_codeword_samples(sample_rate, symbol_rate, codeword):
    codeword_diff = codeword[:-1] ^ codeword[1:] # precoding
    codeword_diff[1::2] ^= 1
    samples_per_symbol = sample_rate // symbol_rate
    taps = np.convolve(gaussian_taps(0.5, samples_per_symbol, 4*samples_per_symbol), np.ones(samples_per_symbol)) # gaussian taps
    codeword_interp = np.zeros(codeword_diff.size * samples_per_symbol)
    codeword_interp[::samples_per_symbol] = 2*codeword_diff.astype('float')-1
    f_samps = scipy.signal.lfilter(taps, 1, codeword_interp)[samples_per_symbol:]
    sensitivity = (np.pi / 2) / samples_per_symbol
    phase = np.cumsum(f_samps) * sensitivity
    return np.exp(1j*phase)



if __name__ == "__main__":
    N = 64
    symbol_rate = 250
    sample_rate = 1000
    csm = get_start_sequence() 
    tail = get_tail_sequence()
    codeword = np.random.randint(0,2,N) # bitstream before the actual sequence
    cltu = np.concatenate([csm, codeword, tail])
    gmsk = generate_codeword_samples(sample_rate, symbol_rate, cltu)
    plt.figure()
    plt.grid()
    plt.plot(gmsk.real, 'r', label = 'real part')
    plt.plot(gmsk.imag, 'b', label ='imaginary part')
    plt.xlabel('Time')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()

