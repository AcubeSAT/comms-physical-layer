function plot_spectrum(gmsk_states, rx, preamble_location, title_str)
  Fs = gmsk_states.Fs;
  st = preamble_location + gmsk_states.npreamble*gmsk_states.M;
  sig = rx(st:st+Fs*0.5);
  h = hanning(length(sig))';
  Rx=20*log10(abs(fftshift(fft(sig .* h, Fs))));
  figure;
  plot(-Fs/2:Fs/2-1,Rx);
  grid("minor");
  xlabel('Hz');
  ylabel('dB');
  topy = ceil(max(Rx)/10)*10;
  axis([-4000 4000 topy-50 topy+10])
  title(title_str);
endfunction