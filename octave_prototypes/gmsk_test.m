function sim_out = gmsk_test(sim_in)
  nsym =  sim_in.nsym;
  EbNodB = sim_in.EbNodB;
  verbose = sim_in.verbose;
  Rs = 4800;
  
  gmsk_states.verbose = verbose;
  gmsk_states.coherent_demod = sim_in.coherent_demod;
  gmsk_states.phase_track = 0;
  gmsk_states = gmsk_init(gmsk_states, Rs);
  M = gmsk_states.M;
  Fs = gmsk_states.Fs;
  Rs = gmsk_states.Rs;
  Bfm = gmsk_states.fm_states.Bfm;
 
  for ne = 1:length(EbNodB)
    aEbNodB = EbNodB(ne);
    EbNo = 10^(aEbNodB/10);
    variance = Fs/(Rs*EbNo);

    tx_bits = round(rand(1, nsym));
    %tx_bits = ones(1, nsym);
    %tx_bits = zeros(1, nsym);
    %tx_bits(1:2:nsym) = 0;
    [tx tx_filt tx_symbols] = gmsk_mod(gmsk_states, tx_bits);
    nsam = length(tx);
    
    noise = sqrt(variance/2)*(randn(1,nsam) + j*randn(1,nsam));
    rx    = tx*exp(j*pi/2) + noise;

    [rx_bits rx_out rx_filt] = gmsk_demod(gmsk_states, rx(1:length(rx)));

    % search for frame location over a range

    Nerrs_min = nsym; 
    Nbits_min = nsym; 
    l = length(rx_bits);
    for i=1:100;
      Nerrs = sum(xor(rx_bits(i:l), tx_bits(1:l-i+1)));
      if Nerrs < Nerrs_min
        Nerrs_min = Nerrs;
        Nbits_min = l;
      end
    end
 
    TERvec(ne) = Nerrs_min;
    BERvec(ne) = Nerrs_min/Nbits_min;
    
    if verbose > 0
      printf("EbNo dB: %3.1f Nerrs: %d BER: %f BER Theory: %f\n", aEbNodB, Nerrs_min, BERvec(ne), 0.5*erfc(sqrt(0.75*EbNo)));
    end

    if verbose > 1

      if gmsk_states.coherent_demod == 0
        Toff = 0; dsam = M*30;
        figure;
        clf
        eyesyms = 2;
        plot(rx_filt(dsam+1+Toff:dsam+eyesyms*M+Toff))
        hold on;
        for i=1:10
          st = dsam+1+Toff+i*eyesyms*M;
          en = st + eyesyms*M;
          plot(rx_filt(st:en))
        end
        hold off;
        %axis([dsam dsam+eyesyms*M -2 2]);
        title('Eye Diagram');
      else
        figure;
        nplot = 16;
        clf;
        subplot(211)
        plot(real(rx_filt(1:nplot*M)))
        axis([1 nplot*M -1 1])
        title('Matched Filter');
        subplot(212)
        plot(imag(rx_filt(1:nplot*M)))
        axis([1 nplot*M -1 1])

        figure;
        nplot = 16;
        clf;
        subplot(211)
        plot(real(rx_out(1:nplot*M))/(2*M))
        title('Integrator');
        axis([1 nplot*M -1 1])
        subplot(212)
        plot(imag(rx_out(1:nplot*M)/(2*M)))
        axis([1 nplot*M -1 1])
     end

      figure;
      clf
      subplot(211)
      stem(tx_bits(1:20))
      title('Tx Bits')
      subplot(212)
      stem(rx_bits(1:20))
      title('Rx Bits')

      figure;
      clf
      subplot(211);
      f = fft(rx);
      Tx = 20*log10(abs(f));
      plot(Tx)
      grid;
      title('GMSK Demodulator Input Spectrum');
      axis([1 5000 0 80])

      subplot(212)
      f = fft(tx);
      f = f(1:length(f)/2);
      cs = cumsum(abs(f).^2);
      plot(cs)
      hold on;
      x = 0.99;
      tots = x*sum(abs(f).^2);
      xpercent_pwr = find(cs > tots);
      bw = 2*xpercent_pwr(1);
      plot([1 Fs/2],[tots tots],'r')
      plot([bw/2 bw/2],[0 tots],'r')
      hold off;  
      title("Cumulative Power");
      grid;
      axis([1 5000 0 max(cs)])

      printf("Bfm: %4.0fHz %3.0f%% power bandwidth %4.0fHz = %3.2f*Rb\n", Bfm, x*100, bw, bw/Rs);

    end
  end

  sim_out.TERvec = TERvec;
  sim_out.BERvec = BERvec;
  sim_out.Rs = gmsk_states.Rs;
endfunction