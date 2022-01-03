% attempt to perform "coarse sync" sync with the received frames, we
% check each frame for the best coarse sync position.  Brute force
% approach, that would be changed for a real demod which has some
% sort of unique word.  Start looking for valid frames 1 frame
% after start of pre-amble to give PLL time to lock

function [total_errors total_bits Nerrs_log Nerrs_all_log errors_log] = coarse_sync_ber(nframes_rx, tx_frame, rx_bits)

  Nerrs_log = zeros(1, nframes_rx);
  Nerrs_all_log = zeros(1, nframes_rx);
  total_errors = 0;
  total_bits   = 0;
  framesize = length(tx_frame);
  errors_log = [];

  for f=2:nframes_rx-1
    Nerrs_min = framesize;
    for i=1:framesize;
      st = (f-1)*framesize+i; en = st+framesize-1;
      errors = xor(rx_bits(st:en), tx_frame); 
      Nerrs = sum(errors);
      if Nerrs < Nerrs_min
        Nerrs_min = Nerrs;
        errors_min = errors;
      end
    end
    Nerrs_all_log(f) = Nerrs_min;
    if Nerrs_min/framesize < 0.1
      errors_log = [errors_log errors_min];
      Nerrs_log(f) = Nerrs_min;
      total_errors += Nerrs_min;
      total_bits   += framesize;
    end
  end
endfunction