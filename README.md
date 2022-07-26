# Physical Layer

* GMSK Prototypes
    * Modulation 
    * Decoder 
    * Symbol synchronization
    * Frame detection
    * Channel Impaiments
* OQPSK Modulation
* BCH Decoder
* LDPC Encoder
* Convolutional Encoder

The files contained in the `octave/` folder are part of the [codec2 low bit rate speech decoder](https://github.com/drowe67/codec2). All of the functions are copied from the [fm.m](https://github.com/drowe67/codec2/blob/master/octave/fm.m) and [gmsk.m](https://github.com/drowe67/codec2/blob/master/octave/gmsk.m) files of the project and are separated into distinct files. They are used for GMSK encoding and decoding as well as testing and plotting.

The  gmsk_protoype.py contains code that is based on the [jupyter_notebooks repository](https://github.com/daniestevez/jupyter_notebooks) and mainly on the [gmsk_dslwp.py](https://github.com/daniestevez/jupyter_notebooks/blob/master/dslwp/dslwp_gmsk.py) GMSK detector script.


* GMSK synchronization cpp implementation (WIP)

