function run_test_channel_impairments
  Rs = 1200;
  verbose = 1;
  aEbNodB = 6;
  phase_offset = pi/2;
  freq_offset  = -104;
  timing_offset = 100E3;
  sample_clock_offset_ppm = -500;
  interferer_freq = -1500;
  interferer_amp  = 0;
  nsym = 4800*2;
  npreamble = 480;

  gmsk_states.npreamble = npreamble;
  gmsk_states.verbose = verbose;
  gmsk_states.coherent_demod = 1;
  gmsk_states.phase_track    = 1;
  gmsk_states = gmsk_init(gmsk_states, Rs);
  Fs = gmsk_states.Fs;
  Rs = gmsk_states.Rs;
  M  = gmsk_states.M;

  % A frame consists of nsym random data bits.  Some experimentation
  % has shown they must be random-ish data (not say 11001100...) for
  % timing estimator to work.  However initial freq offset estimation
  % is a lot easier with a 01010 type sequence, so we construct a 
  % frame with a pre-amble followed by frames of random data.

  framesize = 480;
  nframes = floor(nsym/framesize);
  tx_frame = round(rand(1, framesize));
  tx_bits = zeros(1,npreamble);
  tx_bits(1:2:npreamble) = 1;
  for i=1:nframes
    tx_bits = [tx_bits tx_frame];
  end

  [tx tx_filt tx_symbols] = gmsk_mod(gmsk_states, tx_bits);

  tx = resample(tx, 1E6, 1E6-sample_clock_offset_ppm);
  tx = [zeros(1,timing_offset) tx];
  nsam = length(tx);

  if verbose > 1
    figure;
    subplot(211)
    st = timing_offset; en = st+M*10;
    plot(real(tx(st:en)))
    title('Real part of tx');
    subplot(212)
    plot(imag(tx(st:en)))
    title('Imag part of tx');
  end

  EbNo = 10^(aEbNodB/10);
  variance = Fs/(Rs*EbNo);
  noise = sqrt(variance/2)*(randn(1,nsam) + j*randn(1,nsam));
  w  = (0:nsam-1)*2*pi*freq_offset/Fs + phase_offset;
  interferer = interferer_amp*exp(j*interferer_freq*(2*pi/Fs)*(0:nsam-1));

  rx = sqrt(2)*tx.*exp(j*w) + noise + interferer;
  
  % optional dump to file

  if 1
    fc = 1500; gain = 10000;
    wc = 2*pi*fc/Fs;
    w1 = exp(j*wc*(1:nsam));
    rx1 = gain*real(rx .* w1);
    fout = fopen("rx_6dB.raw","wb");
    fwrite(fout, rx1, "short");
    fclose(fout);
  end

  rx = rx1 .* conj(w1);

  [preamble_location freq_offset_est] = find_preamble(gmsk_states, M, npreamble, rx);
  w_est  = (0:nsam-1)*2*pi*freq_offset_est/Fs;
  rx = rx.*exp(-j*w_est);

  plot_spectrum(gmsk_states, rx, preamble_location, "GMSK rx just after preamble");

  % printf("ntx: %d nrx: %d ntx_bits: %d\n", length(tx), length(rx), length(tx_bits));

  [rx_bits rx_out rx_filt] = gmsk_demod(gmsk_states, rx(preamble_location+framesize:nsam));
  nframes_rx = length(rx_bits)/framesize;

  % printf("ntx: %d nrx: %d ntx_bits: %d nrx_bits: %d\n", length(tx), length(rx), length(tx_bits), length(rx_bits));

  [total_errors total_bits Nerrs_log Nerrs_all_log] = coarse_sync_ber(nframes_rx, tx_frame, rx_bits);

  ber = total_errors/total_bits;

  printf("Eb/No: %3.1f f_off: %4.1f ph_off: %4.3f Nframes: %d Nbits: %d Nerrs: %d BER: %f\n", 
         aEbNodB, freq_offset, phase_offset, nframes_rx, total_bits, total_errors, ber);

  figure;
  clf
  subplot(211)
  plot(Nerrs_log,'r;errors/frame counted for BER;');
  hold on;
  plot(Nerrs_all_log,'g;all errors/frame;');
  hold off;
  legend("boxoff");
  title('Bit Errors')
  subplot(212)
  stem(real(cumsum(Nerrs_log)))
  title('Cumulative Bit Errors')

endfunction