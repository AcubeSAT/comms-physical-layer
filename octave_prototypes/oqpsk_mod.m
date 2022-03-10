% Unfiltered OQPSK modulator 
function [tx tx_symb] = oqpsk_mod(oqpsk_states, tx_bits)
  M = oqpsk_states.M;
  bps = oqpsk_states.bps;
  nsym = length(tx_bits)/bps;
  nsam = nsym*M;
  verbose = oqpsk_states.verbose;
  % Map bits to Gray coded QPSK symbols
  tx_symb = zeros(1,nsym);
  for i=1:nsym
    tx_symb(i) = qpsk_mod(tx_bits(2*i-1:2*i))*exp(j*pi/4);
  end
  % Oversample by M (sample and hold) to create unfiltered QPSK
  tx = zeros(1, nsam);
  for i=1:nsym
    tx((i-1)*M+1:(i*M)) = tx_symb(i);
  end
  % delay Q arm by half of a symbol to make OQPSK
tx = [real(tx) zeros(1,M/2)] + j*[zeros(1,M/2) imag(tx)];
endfunction
