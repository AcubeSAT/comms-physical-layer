import matplotlib.pyplot as plt
import numpy as np

with open('iofiles/acq.txt') as f:
    a = f.read().split()
print(a)
print("\n\n")
b = []
for i in range(len(a)):
    b.append(float(a[i]))

acq = np.array([b[256*i:(256*i+256)] for i in range(480)])
print(np.array(acq))
print(type(acq))

# Correlation Plot
plt.figure()
plt.grid()
plt.plot(np.max(acq, axis=1), 'g-')

plt.show()

# FFT Data from the C++ Implementation
fftc = [0, 0.835493, 0.941979, 0.203869, 0.765367, 1.1121, 0.460249, 0.71871, 1.41421, 0.87575, 0.688812, 2.08059, 1.84776, 0.672066, 4.73565, 8.4829, 10, 8.4829, 4.73565, 0.672066, 1.84776, 2.08059, 0.688812, 0.87575, 1.41421, 0.71871, 0.460249, 1.1121, 0.765367, 0.203869, 0.941979, 0.835493]
a = [0] * 10
b = [1] * 10
c = [0] * 12
sqwave = np.concatenate([a, b, c])
fft = np.abs(np.fft.fftshift(np.fft.fft(sqwave)))
plt.plot(fftc, 'b-', label='C++')
plt.plot(fft, 'r--', label='NumPy')
plt.legend()
plt.show()
