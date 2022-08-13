
This repository contains the Physical and Coding Layer as defined in the CCSDS Layered model. More specifically, this implements:

* Pre-coded GMSK Modulation ([CCSDS 413.0-G-3](https://public.ccsds.org/Pubs/413x0g3e1.pdf))
* OQPSK Modulation ([CCSDS 413.0-G-3](https://public.ccsds.org/Pubs/413x0g3e1.pdf))
* BCH Decoder ([CCSDS 231.0-B-4](https://public.ccsds.org/Pubs/231x0b4e0.pdf))
* LDPC Encoder ([CCSDS 130.1-G-3](https://public.ccsds.org/Pubs/130x1g3.pdf))
* Convolutional Encoder ([CCSDS 130.1-G-3](https://public.ccsds.org/Pubs/130x1g3.pdf))

## Error Correction Codes

### Convolutional Encoder 

The [CCSDS 130.1-G-3](https://public.ccsds.org/Pubs/130x1g3.pdf) (7,1/2) Convolutional Encoder (meaning rate r = 1/2 and constraint length k = 7) is implemented. 
The Convolutional Encoder is shown in the image below. 
It consists of a shift register and some exclusive OR gates that implement the two parity checks. 
Preceding bits are considered to be 0.
![convolutional encoder](/media/convolutionalEncoder.png "The Convolutional Encoder")

#### Class Definition
 
The encoder is defined in ```inc/Convolutional.hpp```

#### Methods

```cpp
void encode(const etl::bitset<inputLength>& input, etl::bitset<outputLength>& output);
```

* ```input``` : Provide the input to the encoder in the form of a reference to an etl::bitset of size ```inputLength```
* ```output``` : Provide the input to the encoder in the form of a reference to an etl::bitset of size ```outputLength```

### LDPC Encoder

The [CCSDS 130.1-G-3](https://public.ccsds.org/Pubs/130x1g3.pdf) LDPC Encoder at rate of r = 4/5 with information block length k = 4096 (encoded size is then 5120) is implemented.

#### Class Definition

The encoder is defined in ```inc/LDPCEncoder.hpp```. ```inc/PostionRowsEsoteric.hpp``` and ```inc\RowsParityBitsEsoteric.hpp``` are also used in the implementation.

#### Methods

```cpp
void encode(const etl::bitset<inputLength>& input, etl::bitset<outputLength>& output);
```

* ```input``` : Provide the input to the encoder in the form of a reference to an etl::bitset of size ```inputLength```
* ```output``` : Provide the input to the encoder in the form of a reference to an etl::bitset of size ```outputLength```

## Modulations

The GMSK and OQPSK modulation were mostly ported to C++ from the [codec2](https://github.com/drowe67/codec2) project by [Rowetel](https://www.rowetel.com/).

### FM Modulation

As part of the GMSK modulator, an FM modulator is also provided:


#### Class definition

```cpp
FMTranscoder(uint32_t samplingFrequency, uint32_t maxModulatingFrequency, uint32_t basebandFrequency, uint16_t maxDeviation, bool preDeEmphasis, bool limitDeltaPhase)
```

* `samplingFrequency`: Sample frequency of the signal
* `maxModulatingFrequency`: Highest frequency component present in the modulated signal
* `basebandFrequency`: Baseband frequency (usually set to 0)
* `maxDeviation`: Maximum deviation from the carrier frequency
* `preDeEmphasis`: Defines whether pre- and and de-emphasis filters of high frequencies is applied

#### Methods

```cpp
modulate(const double *signal, uint16_t signal_length, double *in_phase_signal, double *quadrature_signal)
```

* `signal`: Pointer to the input signal to be modulated
* `signal_length`: Input signal length 
* `in_phase_signal`: Pointer to output in-phase component 
* `quadrature_signal`: Pointer to output quadrature component


### GMSK Transcoder

> **Warning:** 
> The class internally initializes internal buffers (**size TBC** with configurable sps) for calculations that can't happen in-place. So take this into account that if you have severe space restrictions, this will take-up a non-negligible amount of memory


The GMSK modulator also automatically uses NRZ encoding in the input to minimize the BER on the receiving end. The filter on the transceiver uses pre-coded taps for BTs = 0.5 (for more information refer to CCSDS 413.0-G-3 Section 3.1)


#### Class definition
```cpp 
GMSKTranscoder(uint32_t sampleFrequency, uint32_t symbolRate, bool equalize)
```
 
 * `sampleFrequency`: Sampling frequency 
 * `symbolRate`: Symbol rate 
 * `equalize`: Whether equalization will be used in the demodulator to offset the effects of the (non-Nyquist) Gaussian filter used in the modulator

#### Methods

```cpp
modulate(bool *signal, uint16_t signalLength, double *inPhaseSignal, double *quadratureSignal)
```
* `signal`: Pointer to the input signal to be modulated
* `signalLength`: Input signal signal length 
* `inPhaseSignal`: Pointer to output in-phase component 
* `quadratureSignal`: Pointer to output quadrature component

```cpp
demodulate(double *inputInPhaseSignal, double *inputQuadratureSignal, uint16_t signalLength, bool *signal)
```
* `inputInPhaseSignal`: Pointer to input in-phase component 
* `inputQuadratureSignal`: Pointer to input quadrature component
* `signalLength`: Input signal length 
* `signal`: Pointer to output signal

### OQPSK Transcoder

> **Warning:** 
> The class internally initializes internal buffers (**size TBC** with configurable sps) for calculations that can't happen in-place. So take this into account that if you have severe space restrictions, this will take-up a non-negligible amount of memory

#### Class definition
```cpp
OQPSKTranscoder(uint32_t samplingFrequency, uint32_t symbolRate)
```

* `sampleFrequency`: Sampling frequency 
* `symbolRate`: Symbol rate 

#### Methods

```cpp
modulate(bool *signal, uint16_t signalLength, double *inPhaseSignal, double *quadratureSignal)
```
* `signal`: Pointer to the input signal to be modulated
* `signalLength`: Input signal signal length 
* `inPhaseSignal`: Pointer to output in-phase component 
* `quadratureSignal`: Pointer to output quadrature component


### Synchronization


The  gmsk_protoype.py contains code that is based on the [jupyter_notebooks repository](https://github.com/daniestevez/jupyter_notebooks) and mainly on the [gmsk_dslwp.py](https://github.com/daniestevez/jupyter_notebooks/blob/master/dslwp/dslwp_gmsk.py) GMSK detector script.


