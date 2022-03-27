Rs=220000;
oqpsk_states=oqpsk_init(oqpsk_states,Rs);
tx_bits=[0, 0, 0, 1, 1, 0, 1, 1];
[tx tx_symb]=oqpsk_mod(oqpsk_states, tx_bits )
