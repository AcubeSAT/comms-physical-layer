sim_in.precoding = 1;
sim_in.nsym = 10000;
sim_in.EbNodB = 2:10;
sim_in.verbose = 1;

gmsk_coh = gmsk_test(sim_in);

sim_in.precoding = 0;

gmsk_noncoh = gmsk_test(sim_in);

Rs = gmsk_coh.Rs;
EbNo  = 10 .^ (sim_in.EbNodB/10);
alpha = 0.75; % guess for BT=0.5 GMSK
gmsk_theory.BERvec = 0.5*erfc(sqrt(alpha*EbNo));

% BER v Eb/No curves

figure;
clf;
semilogy(sim_in.EbNodB, gmsk_theory.BERvec,'r;GMSK theory;')
hold on;
semilogy(sim_in.EbNodB, gmsk_coh.BERvec,'g;GMSK coherent, without precoder;')
semilogy(sim_in.EbNodB, gmsk_noncoh.BERvec,'b;GMSK coherent with precoder;')
hold off;
grid("minor");
axis([min(sim_in.EbNodB) max(sim_in.EbNodB) 1E-4 1])
legend("boxoff");
xlabel("Eb/No (dB)");
ylabel("Bit Error Rate (BER)")

% BER v C/No (1 Hz noise BW and Eb=C/Rs=1/Rs)
% Eb/No = (C/Rs)/(1/(N/B))
% C/N   = (Eb/No)*(Rs/B)

RsOnB_dB = 10*log10(Rs/1);
figure;
clf;
semilogy(sim_in.EbNodB+RsOnB_dB, gmsk_theory.BERvec,'r;GMSK theory;')
hold on;
semilogy(sim_in.EbNodB+RsOnB_dB, gmsk_coh.BERvec,'g;GMSK coherent, without precoder;')
semilogy(sim_in.EbNodB+RsOnB_dB, gmsk_noncoh.BERvec,'b;GMSK coherent with precoder;')
hold off;
grid("minor");
axis([min(sim_in.EbNodB+RsOnB_dB) max(sim_in.EbNodB+RsOnB_dB) 1E-4 1])
legend("boxoff");
xlabel("C/No for Rs=4800 bit/s and 1 Hz noise bandwidth (dB)");
ylabel("Bit Error Rate (BER)");