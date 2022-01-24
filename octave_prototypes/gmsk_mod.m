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

tx_filt = filter(gmsk_states.mod_coeff, 1, tx_symbols);

if verbose > 1
    figure;
    clf
    plot(tx_filt(1:M*10))
    title('tx signal after filtering, before FM mod')
end

tx = analog_fm_mod(gmsk_states.fm_states, tx_filt);
end


