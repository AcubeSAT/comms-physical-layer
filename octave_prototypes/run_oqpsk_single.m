sim_in.nbits   = 100;
nbits =  sim_in.nbits;
Rs = 10000;
oqpsk_states=[];
oqpsk_states = oqpsk_init(oqpsk_states, Rs);

M = oqpsk_states.M;
Fs = oqpsk_states.Fs;
Rs = oqpsk_states.Rs;
tx_bits = round(rand(1,nbits));

[tx tx_symb] = oqpsk_mod(oqpsk_states, tx_bits);
figure()
plot(real(tx_symb));