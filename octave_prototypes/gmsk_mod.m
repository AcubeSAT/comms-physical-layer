function [tx tx_filt tx_symbols] = gmsk_mod(gmsk_states, tx_bits)
M = gmsk_states.M;
nsym = length(tx_bits);
nsam = nsym*M;
verbose = gmsk_states.verbose;

% NRZ sequence of symbols

tx_symbols = zeros(1,nsam);
for i=1:nsym
    tx_symbols(1+(i-1)*M:i*M) = -1 + 2*tx_bits(i);
end

if(gmsk_states.precoding==0)
  tx_bits1=shift(tx_symbols,M);
  tx_bits1(1:M)=1;
    for i = 1:nsym
      tx_clock(1+(i-1)*M:i*M) = (-1)^(i);
    end
    tx_symbols=tx_symbols.*tx_bits1.*tx_clock;
end

tx_filt = filter(gmsk_states.mod_coeff, 1, tx_symbols);

tx = analog_fm_mod(gmsk_states.fm_states, tx_filt);
end


