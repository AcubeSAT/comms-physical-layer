sim_in.coherent_demod = 0;
sim_in.nsym = 4800;
sim_in.EbNodB = 10;
sim_in.verbose = 2;
sim_out = gmsk_test(sim_in);
