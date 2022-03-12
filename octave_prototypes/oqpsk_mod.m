function [tx tx_symb] = oqpsk_mod(oqpsk_states, tx_bits)
  M = oqpsk_states.M;
  bps = oqpsk_states.bps;
  nsym = length(tx_bits)/bps;
  nsam = nsym*M;
  tx_symb = zeros(1,nsym);
  for i=1:nsym
    tx_symb(i) = qpsk_mod(tx_bits(2*i-1:2*i))*exp(j*pi/4);
  end
  tx = zeros(1, nsam);
  for i=1:nsym
    tx((i-1)*M+1:(i*M)) = tx_symb(i);
  end
txn = [real(tx) zeros(1,M/2)] + j*[zeros(1,M/2) imag(tx)];
tx = filter(oqpsk_states.rrcfilter, 1, txn);
figure()
plot(real(tx))
hold on
plot(imag(tx))
hold on 
plot(sqrt(real(tx).^2+imag(tx).^2))
endfunction
