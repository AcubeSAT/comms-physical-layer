% Reads a file of Fs=48kHz 16 bit samples centred on 1500Hz, and
% measures the BER.

function gmsk_rx(rx_file_name, err_file_name)
  rand('state',1); 

  Rs = 1200;
  framesize = 480;
  npreamble = 480;
  fc        = 1500;

  gmsk_states.npreamble = npreamble;
  gmsk_states.verbose = 1;
  gmsk_states.coherent_demod = 1;
  gmsk_states.phase_track    = 1;
  gmsk_states = gmsk_init(gmsk_states, Rs);
  Fs = gmsk_states.Fs;
  Rs = gmsk_states.Rs;
  M  = gmsk_states.M;

  tx_frame = round(rand(1, framesize));

  % get real signal at fc offset and convert to baseband complex
  % signal
  
  fin = fopen(rx_file_name,"rb");
  rx = fread(fin,"short")';
  fclose(fin);
  rx = filter([1 -0.999],[1 -0.99],rx);
  nsam = length(rx);
  wc = 2*pi*fc/Fs;
  w = exp(-j*wc*(1:nsam));
  rxbb = rx .* w;
 
  figure;
  plot(rx);

  % find preamble

  [preamble_location freq_offset_est] = find_preamble(gmsk_states, M, npreamble, rxbb);

  % power of signal, averaged over window
  % TODO: remove wave file header, scale of actual level
  %       filter so we measure only energy in our passband
  %       work out noise BW of filter.  Use GMSK filter?

  [b a] = cheby2(6,40,[200 3000]/(Fs/2));
  %bpwr_lp  = fir2([200,4000/(Fs/2));
  noise_bw = var(filter(b,a,randn(1,1E6)));

  rx_filt = filter(b, a, rx(1000:length(rx)));
  npower_window = 200*M;
  rx_power = conv(rx_filt.^2,ones(1,npower_window))/(npower_window);
  rx_power_dB = 10*log10(rx_power);
  figure;
  subplot(211)
  plot(rx_filt(1000:length(rx_filt)));
  title('GMSK Power (narrow filter)');
  subplot(212)
  plot(rx_power_dB);
  axis([1 length(rx_power) max(rx_power_dB)-29 max(rx_power_dB)+1])
  grid("minor")

  % Work out where to sample N, and S+N

  noise_end = preamble_location - 2*npreamble*M;
  noise_start = noise_end - Fs;
  if noise_start < 1
    printf("Hmm, we really need >1 second of noise only before preamble to measure noise!\n");
  else
    noise = mean(rx_power_dB(noise_start:noise_end));
    signal_noise_start = preamble_location + 2*npreamble*M;
    signal_noise_end = signal_noise_start + Fs;
    signal_noise = mean(rx_power_dB(signal_noise_start:signal_noise_end));
    hold on;
    plot([noise_start noise_end],[noise noise],'color','r','linewidth',5);
    plot([signal_noise_start signal_noise_end],[signal_noise signal_noise],'color','r','linewidth',5);

    % determine SNR
  
    noise_lin = 10 ^ (noise/10);
    signal_noise_lin = 10 ^ (signal_noise/10);
    signal_lin = signal_noise_lin - noise_lin;
    signal = 10*log10(signal_lin);
    snr = signal - noise;
    fudge_factor = 3;                           % 3dB for single/double sided noise adjustment?  Just a guess
    CNo = snr + 10*log10(Fs*noise_bw) - fudge_factor;
    EbNo = CNo - 10*log10(Rs);  

    EbNo_lin  = 10 .^ (EbNo/10);
    alpha = 0.75;                             % guess for BT=0.5 GMSK
    ber_theory = 0.5*erfc(sqrt(alpha*EbNo_lin));

    printf("Estimated S: %3.1f N: %3.1f Nbw: %4.0f Hz SNR: %3.1f CNo: %3.1f EbNo: %3.1f BER theory: %f\n",
           signal, noise, Fs*noise_bw, snr, CNo, EbNo, ber_theory);

  % FM signal is centred on 12 kHz and 16 kHz wide so lets also work out noise there

  [b a] = cheby2(6,40,[12000-8000 12000+8000]/(Fs/2));
  noise_bw_fm = var(filter(b,a,randn(1,1E6)));

  rx_filt_fm = filter(b, a, rx(1000:length(rx)));
  rx_power_fm = conv(rx_filt_fm.^2,ones(1,npower_window))/(npower_window);
  rx_power_dB_fm = 10*log10(rx_power_fm);
  
  noise = mean(rx_power_dB_fm(noise_start:noise_end))*ones(1, length(rx_power_fm));
  noise_lin = 10 .^ (noise/10);

  signal_lin = rx_power_fm - noise_lin;
  signal = 10*log10(abs(signal_lin) + 1E-6);
  snr = signal - noise;

  CNo = snr + 10*log10(Fs*noise_bw_fm) - fudge_factor;

  figure
  plot(rx_power_dB_fm,'r;signal plus noise;');
  hold on;
  plot(CNo,'g;C/No;');
  hold off;
  top_fm = ceil(max(CNo)/10)*10;
  axis([1 length(rx_power_dB_fm) 20 top_fm])
  grid("minor")
  legend("boxoff");
  title('FM C/No');
  end

  % spectrum of a chunk of GMSK signal just after preamble

  plot_spectrum(gmsk_states, rx, preamble_location, "GMSK rx just after preamble");

  % correct freq offset and demodulate

  w_est  = (0:nsam-1)*2*pi*freq_offset_est/Fs;
  rxbb = rxbb.*exp(-j*w_est);
  st = preamble_location+npreamble*M; 
  %en = min(nsam,st + 4*framesize*M); 
  en = nsam;
  gmsk_statres.verbose = 2;
  [rx_bits rx_out rx_filt] = gmsk_demod(gmsk_states, rxbb(st:en));
  nframes_rx = length(rx_bits)/framesize;

  % count errors

  [total_errors total_bits Nerrs_log Nerrs_all_log errors_log] = coarse_sync_ber(nframes_rx, tx_frame, rx_bits);

  ber = total_errors/total_bits;

  printf("Nframes: %d Nbits: %d Nerrs: %d BER: %f\n", 
         nframes_rx, total_bits, total_errors, ber);

  % Optionally save a file of bit errors so we can simulate the effect on Codec 2

  if nargin == 2

    % To simulate effects of these errors on Codec 2:
    %   $  ~/codec2-dev/octave$ ../build_linux/src/c2enc 1300 ../raw/hts1raw - | ../build_linux/src/insert_errors - - ssb7dbSNR.err 52 | ../build_linux/src/c2dec 1300 - - | play -t raw -r 8000 -s -2 -
    % Note in this example I'm using the 1300 bit/s codec, it's sig more robust that 1200 bit/s, 
    % if we ran the GMSK modem at 1300 bit/s there would be a 10*log10(1300/1200) = 0.35dB SNR penalty 

    fep=fopen(err_file_name,"wb"); fwrite(fep, errors_log, "short"); fclose(fep);
  end

  figure;
  clf
  subplot(211)
  plot(Nerrs_log,'r;errors/frame counted for BER;');
  hold on;
  plot(Nerrs_all_log,'g;all errors/frame;');
  hold on;
  title('Bit Errors')
  legend("boxoff");
  subplot(212)
  stem(real(cumsum(Nerrs_log)))
  title('Cumulative Bit Errors')
endfunction