sim_in.coherent_demod = 0;
sim_in.nsym = 10;
sim_in.EbNodB = 10;
sim_in.verbose = 2;
nsym =  sim_in.nsym;
EbNodB = sim_in.EbNodB;
verbose = sim_in.verbose;
Rs = 4800;
gmsk_states.verbose = verbose;
gmsk_states.coherent_demod = sim_in.coherent_demod;
gmsk_states.phase_track = 0;
gmsk_states = gmsk_init(gmsk_states, Rs);
fm_states.pre_emp=1;
tx_bits = round(rand(1, nsym));
%tx_diff=zeros(1,length(tx_bits));
tx_bits = 2*tx_bits-1
tx_bits1=shift(tx_bits,1);
tx_bits1(1)=1;
for i = 1:10
tx_clock(i) = (-1)^(i);
end
tx_bits
tx_bits1
tx_clock
tx_diff=tx_bits.*tx_bits1.*tx_clock
%tx_new(2:end)=tx_diff
%tx_new(2:end)=tx_diff
%tx_bits = ones(1, nsym);
%tx_bits = zeros(1, nsym);
%tx_bits(1:2:nsym) = 0;
%tx_diff=tx_diff<0
tx_init=tx_bits;
[tx, tx_filt, tx_symbols] = gmsk_mod(gmsk_states, tx_bits);
figure()
plot(real(tx))
figure()
plot(imag(tx))
sim_in.coherent_demod = 0;
sim_in.nsym = 10;
sim_in.EbNodB = 10;
sim_in.verbose = 2;
nsym =  sim_in.nsym;
EbNodB = sim_in.EbNodB;
verbose = sim_in.verbose;
Rs = 4800;
gmsk_states.verbose = verbose;
gmsk_states.coherent_demod = sim_in.coherent_demod;
gmsk_states.phase_track = 0;
gmsk_states = gmsk_init(gmsk_states, Rs);
[rx_bits rx_int rx_filt] = gmsk_demod(gmsk_states, tx);
tx_init(3:12)
rx_bits(1:10)
length(tx_bits)
length(rx_bits)