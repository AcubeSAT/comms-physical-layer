function [preamble_location freq_offset_est] = find_preamble(gmsk_states, M, npreamble, rx)
  verbose = gmsk_states.verbose;

  % look through rx buffer and determine if there is a valid preamble.  Use steps of half the
  % preamble size in samples to try to bracket the pre-amble.

  preamble_step = npreamble*M/2;
  ratio = 0; freq_offset_est = 0; preamble_location = 0;
  ratio_log = [];
  for i=1:preamble_step:length(rx)-preamble_step
    [afreq_offset_est aratio] = gmsk_est_freq_offset(gmsk_states, rx(i:i+preamble_step-1), verbose);
    ratio_log = [ratio_log aratio];
    if aratio > ratio
      preamble_location = i;
      ratio = aratio;
      freq_offset_est = afreq_offset_est;
    end
  end
  if verbose
    printf("preamble location: %2.1f seconds est f_off: %5.1f Hz ratio: %3.2f\n", 
    preamble_location/gmsk_states.Fs, freq_offset_est, ratio);   
    figure;
    plot(ratio_log);
    title('Preamble ratio');
  end
endfunction