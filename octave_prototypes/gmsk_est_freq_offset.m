
% Initial frequency offset estimation. Look for line a centre
% frequency, which is the strongest component when ...101010... is
% used to modulate the GMSK signal.  Note just searching for a single
% line will get false lock on random sine waves but that's OK for a
% PoC.  It could be improved by checking for other lines, or
% demodulating the preamble and checking for bit errors.
  
function [freq_offset_est ratio] = gmsk_est_freq_offset(gmsk_states, rx, verbose)
  Fs = gmsk_states.Fs;
  Rs = gmsk_states.Rs;

  % Suggest Rs/10 symbols of preamble (100ms), this works OK at
  % Rs=4800 and Es/No = 6dB.  The large, Fs sample FFT size is used
  % for convenience (the bin resolution is 1 Hz), for real time we
  % would decimate and use smaller FFT to save CPU and memory.
  
  ndft = Fs;
  f = fft(rx .* hanning(length(rx))', ndft);
  f = fftshift(f);

  start_bin = 1 + Fs/2-Rs/4; 
  stop_bin = start_bin + Rs/2;
  [max_val max_bin] = max(abs(f(start_bin:stop_bin)));
  
  max_bin -= Rs/4 + 1;
  if verbose > 1
    printf("ndft: %d start_bin: %d stop_bin: %d max_bin: %d\n", ndft, start_bin, stop_bin, max_bin);
  end

  % calc ratio of line energy to total energy.  For a valid preamble
  % this was measured as about 0.20 to 0.25 depending on noise.

  sum_sq = sum(abs(f(start_bin:stop_bin)) .^ 2);
  ratio = sqrt(max_val*max_val/sum_sq);

  % map max_bin to frequency offset

  freq_offset_est = max_bin;  

  if verbose > 1
    printf("freq_offset_est: %f  pk/rms ratio: %f \n", freq_offset_est, ratio);
    figure;
    clf
    subplot(211)
    plot(rx,'+')
    title('rx signal on complex plane')
    subplot(212)
    plot(-Rs/4:Rs/4, 20*log10(abs(f(start_bin:stop_bin))));
    axis([-Rs/4 Rs/4 0 80]);
    title('spectrum of rx signal');
  end

endfunction