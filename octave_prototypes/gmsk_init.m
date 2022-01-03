function gmsk_states = gmsk_init(gmsk_states, Rs)

  verbose = gmsk_states.verbose;
  gmsk_states.Fs = 48000;
  gmsk_states.Rs = Rs;
  M = gmsk_states.M = gmsk_states.Fs/gmsk_states.Rs;
  global gmsk_mod_coeff;
  global gmsk_demod_coeff;
  gmsk_states.mod_coeff = (Rs/4800)*resample(gmsk_mod_coeff, 4800, Rs);

  if verbose > 1
    figure;
    plot(gmsk_mod_coeff,'r;original 4800;')
    hold on;
    plot(gmsk_states.mod_coeff,'g;interpolated;')
    hold off;
    title('GMSK pulse shaping filter')
  end

  % set up FM modulator

  fm_states.Fs = gmsk_states.Fs;  
  fm_states.fc = 0;  
  fm_max = fm_states.fm_max = Rs/2;
  fd = fm_states.fd = Rs/4;
  fm_states.Ts = gmsk_states.M;  
  fm_states.pre_emp = fm_states.de_emp = 0;
  fm_states.output_filter = 1;
  gmsk_states.fm_states = analog_fm_in(fm_states);
  
endfunction

