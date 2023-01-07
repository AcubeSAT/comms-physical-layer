import matplotlib.pyplot as plt


# TEST CASE: CREATE GMSK SIGNAL
with open('iofiles/gmskSignal.txt') as f:
    signal = f.read().split()

i = []
q = []
for j in range(len(signal)):
    if j % 2 == 0:
        i.append(float(signal[j]))
    else:
        q.append(float(signal[j]))

plt.plot(i[0:160], 'r-')
plt.plot(q[0:160], 'b-')

plt.title("GMSK SIGNAL")
plt.show()

# TEST CASE: ADD WGN TO THE GMSK SIGNAL

with open('iofiles/wgnSignal.txt') as f:
    wgnSignal = f.read().split()

iWGN = []
qWGN = []
for j in range(len(wgnSignal)):
    if j % 2 == 0:
        iWGN.append(float(wgnSignal[j]))
    else:
        qWGN.append(float(wgnSignal[j]))

plt.plot(iWGN[0:160], 'y-', qWGN[0:160], 'g-')
plt.plot(i[0:160], 'r-')
plt.plot(q[0:160], 'b-')
plt.title("WGN added to GMSK Signal")

plt.show()

# TEST CASE: PHASE SHIFT THE GMSK SIGNAL

with open('iofiles/phaseShiftedSignal.txt') as f:
    phaseShiftedSignal = f.read().split()

iPhaseShifted = []
qPhaseShifted = []
for j in range(len(phaseShiftedSignal)):
    if j % 2 == 0:
        iPhaseShifted.append(float(phaseShiftedSignal[j]))
    else:
        qPhaseShifted.append(float(phaseShiftedSignal[j]))

plt.plot(iPhaseShifted[0:160], 'g-')
plt.plot(qPhaseShifted[0:160], 'y-')
plt.plot(i[0:160], 'r-')
plt.plot(q[0:160], 'b-')

plt.title("Phase shifted GMSK signal")

plt.show()

# TEST CASE: FREQUENCY SHIFT THE GMSK SIGNAL

with open('iofiles/freqShiftedSignal.txt') as f:
    freqShiftedSignal = f.read().split()

iFreqShifted = []
qFreqShifted = []
for j in range(len(freqShiftedSignal)):
    if j % 2 == 0:
        iFreqShifted.append(float(freqShiftedSignal[j]))

    else:
        qFreqShifted.append(float(freqShiftedSignal[j]))

plt.plot(iFreqShifted[0:160], 'y-')
plt.plot(qFreqShifted[0:160], 'g-')
plt.plot(i[0:160], 'r-')
plt.plot(q[0:160], 'b-')

plt.title("Frequency shifted GMSK signal")

plt.show()
