import matplotlib.pyplot as plt
import math

with open('iofiles/iqOQPSKrep.txt') as f:
    iq = f.read().split()
with open('iofiles/iqOQPSKrand.txt') as f:
    iqRand = f.read().split()

# Plot BER
snr = [i for i in range(1, 21)]
with open('iofiles/ber.txt') as f:
    berRead = f.read().split()
ber = []
for i in berRead:
    ber.append(float(i))
ber_theory = [0.5*math.erfc(math.sqrt(0.75*i)) for i in snr]

for ebn0, i in enumerate(ber):
    print(f"EbN0 = {ebn0} | BER = {i}")

plt.yscale('log')
plt.plot(snr, ber, 'r-')
plt.grid()
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