import matplotlib.pyplot as plt
import math

with open('iofiles/out_iqOQPSKrep.txt') as f:
    iq = f.read().split()
with open('iofiles/out_iqOQPSKrand.txt') as f:
    iqRand = f.read().split()

# Plot BER
snr = [i for i in range(1, 21)]
with open('iofiles/out_ber10.txt') as f:
    ber10Read = f.read().split()
with open('iofiles/out_ber6.txt') as f:
    ber6Read = f.read().split()

ber10 = []
ber6 = []
ber4 = []
for i in ber10Read:
    ber10.append(float(i))
for i in ber6Read:
    ber6.append((float(i)))

ber_theory = [0.5*math.erfc(math.sqrt(0.75*i)) for i in snr]

for ebn0, i in enumerate(ber10):
    print(f"EbN0 = {ebn0} | BER = {i}")

plt.yscale('log')
plt.plot(snr, ber10, 'r-', label='SPS = 10')
plt.plot(snr, ber6, 'b-', label='SPS = 6')

plt.grid()
plt.legend()
plt.show()

i = []
q = []
for j in range(len(iq)):
    if j % 2 == 0:
        i.append(float(iq[j]))
    else:
        q.append(float(iq[j]))
# print(i[0:100])
# print(q)
# norm = [i[j]**2 + q[j]**2 for j in range(len(i))]

iRand = []
qRand = []
for j in range(len(iqRand)):
    if j % 2 == 0:
        iRand.append(float(iqRand[j]))
    else:
        qRand.append(float(iqRand[j]))

plt.plot(i[0:160], 'r-')
plt.plot(q[0:160], 'b-')
plt.grid()
# plt.plot(norm[0:500], 'g-')
# print(norm)
plt.show()

plt.plot(iRand, qRand)
plt.grid()
plt.show()