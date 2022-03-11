function oqpsk_states = oqpsk_init(oqpsk_states, Rs)
  % general 
  global rrcfilter
  oqpsk_states.Fs  = 16*Rs;
  oqpsk_states.Rs  = Rs;
  oqpsk_states.bps = 2;  
  oqpsk_states.rrcfilter=rrcfilter;
 
 % two bit/symbol for QPSK    
  M = oqpsk_states.M = oqpsk_states.Fs/oqpsk_states.Rs;
  assert(floor(M) == M, "oversampling factor M must be an integer");
  assert(floor(M/2) == M/2, "(oversampling factor M)/2 must be an integer to offset QPSK");
endfunction