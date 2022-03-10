sim_in.nbits            = 10000;
sim_in.EbNodB           = 4;
sim_in.verbose          = 1;
sim_in.phase_offset     = 3*pi/4;  % in radians
sim_in.timing_offset    = 4;       % in samples 0..M-1
sim_in.freq_offset      = 0.001;   % fraction of Symbol Rate
sim_in.plots            = [1 2 4 5 6 7];
nbits =  sim_in.nbits;
EbNodB = sim_in.EbNodB;
verbose = sim_in.verbose;
Rs = 4800;

oqpsk_states.verbose = verbose;
oqpsk_states = oqpsk_init(oqpsk_states, Rs);
M = oqpsk_states.M;
Fs = oqpsk_states.Fs;
Rs = oqpsk_states.Rs;

tx_bits = [];
aEbNodB = EbNodB;
EbNo = 10^(aEbNodB/10);
variance = Fs/(Rs*EbNo*oqpsk_states.bps);
[tx tx_symb] = oqpsk_mod(oqpsk_states, tx_bits);
nsam = length(tx)