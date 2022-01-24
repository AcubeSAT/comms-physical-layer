% Generates a Fs=48kHz raw file of 16 bit samples centred on 1500Hz,
% Suitable for transmitting with a SSB tx

function gmsk_tx(tx_file_name)
rand('state',1);
Rs = 1200;
nsym =  Rs*4;
framesize = 480;
npreamble = 480;
gain      = 10000;
fc        = 1500;

gmsk_states.verbose = 0;
gmsk_states.coherent_demod = 1;
gmsk_states.phase_track    = 1;
gmsk_states = gmsk_init(gmsk_states, Rs);
Fs = gmsk_states.Fs;
Rs = gmsk_states.Rs;
M  = gmsk_states.M;

% generate frame with preamble

nframes = floor(nsym/framesize);
tx_frame = round(rand(1, framesize));
tx_bits = zeros(1,npreamble);
tx_bits(1:2:npreamble) = 1;

for i=1:nframes
    tx_bits = [tx_bits tx_frame];
end

[tx tx_filt tx_symbols] = gmsk_mod(gmsk_states, tx_bits);
nsam = length(tx);

wc = 2*pi*fc/Fs;
w = exp(j*wc*(1:nsam));
tx = gain*real(tx .* w);
figure;
plot(tx(1:4000))
fout = fopen(tx_file_name,"wb");
fwrite(fout, tx, "short");
fclose(fout);

end