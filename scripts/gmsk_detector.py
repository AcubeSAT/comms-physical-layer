import numpy as np
import matplotlib.pyplot as plt
import scipy.io.wavfile
import scipy.signal

# GMSK Detector by Daniel Estevez (https://github.com/daniestevez/jupyter_notebooks/blob/master/dslwp/dslwp_gmsk.py)
# Modified to be compatible to our project and used as a prototype for the Synchronizer class

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


def compute_acq(signal, sample_rate, symbol_rate):
    asm = np.conj(generate_codeword_samples(sample_rate, symbol_rate, get_start_sequence()))
    step = int((sample_rate/symbol_rate)/4)
    acq = np.zeros(((signal.size-asm.size)//step, asm.size))
    for j, offset in enumerate(range(0,(signal.size-asm.size)//step*step,step)):
        acq[j,:] = np.abs(np.fft.fftshift(np.fft.fft(signal[offset:offset+asm.size] * asm)))**2
    return acq, step


def main():
    N = 64
    symbol_rate = 100
    sample_rate = 1000
    csm = get_start_sequence()
    tail = get_tail_sequence()
    codeword = np.random.randint(0,2,N) # bitstream before the actual sequence
    cltu = np.concatenate([np.random.randint(0, 2, 16), csm, codeword, tail])
    not_cltu = np.random.randint(0, 2, len(cltu))
    print(cltu)
    print(not_cltu)
    gmsk = generate_codeword_samples(sample_rate, symbol_rate, cltu)

    acq, step = compute_acq(generate_codeword_samples(sample_rate, symbol_rate, cltu), sample_rate, symbol_rate)
    acq2, step2 = compute_acq(generate_codeword_samples(sample_rate, symbol_rate, not_cltu), sample_rate, symbol_rate)
    print(acq)

    tsync = np.argmax(np.max(acq, axis=1))
    fsync = np.argmax(acq[tsync,:])
    ts = np.arange(acq.shape[0]) * step / sample_rate
    fs = np.arange(-acq.shape[1]//2, acq.shape[1]//2) * sample_rate / acq.shape[1]

    ts2 = np.arange(acq2.shape[0]) * step2 / sample_rate
    fs2 = np.arange(-acq2.shape[1]//2, acq2.shape[1]//2) * sample_rate / acq2.shape[1]
    tsync2 = np.argmax(np.max(acq2, axis=1))

    plt.figure()
    plt.grid()
    plt.plot(ts, np.max(acq, axis=1), 'g', label = 'ACQ')
    plt.xlabel('Time (s)')
    plt.ylabel('Correlation')
    plt.title('Contains ASM')
    plt.legend()

    plt.figure()
    plt.grid()
    plt.plot(ts2, np.max(acq2, axis=1), 'g', label = 'ACQ')
    plt.xlabel('Time (s)')
    plt.ylabel('Correlation')
    plt.title('Does not contain ASM')
    plt.legend()

    plt.figure()
    plt.grid()
    plt.plot(fs, acq[tsync,:])
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Correlation')
    plt.title('Contains ASM')
    plt.legend()

    plt.figure()
    plt.grid()
    plt.plot(fs2, acq2[tsync2,:])
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Correlation')
    plt.title('Does not contain ASM')
    plt.legend()

    plt.show()

    plt.figure()
    plt.grid()
    plt.plot(gmsk.real, 'r', label = 'real part')
    plt.plot(gmsk.imag, 'b', label ='imaginary part')
    plt.xlabel('Time')
    plt.ylabel('Amplitude')
    plt.legend()
    plt.show()


if __name__ == "__main__":
    main()
