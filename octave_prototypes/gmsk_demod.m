function [rx_bits rx_int rx_filt] = gmsk_demod(gmsk_states, rx)
  M = gmsk_states.M;
  Rs = gmsk_states.Rs;
  Fs = gmsk_states.Fs;
  nsam = length(rx);
  nsym = floor(nsam/M);
  global gmsk_demod_coeff;
  wd = 2*pi*gmsk_states.fm_states.fd/gmsk_states.Fs;
  timing_angle_log = zeros(1,length(rx));
  rx_int = zeros(1,length(rx));

  if gmsk_states.coherent_demod

    % See IEEE Trans on Comms, Muroyta et al, 1981, "GSM Modulation
    % for Digital Radio Telephony" Fig 8:

    % matched filter

    rx_filt = filter(gmsk_states.mod_coeff, 1, rx);

    % Property of MSK that re and im arms are sequences of 2T
    % long symbols, can be demodulated like QPSK with matched filter
    % and integrate and dump.

    % integrate energy in symbols 2T long in re and im arms
    % note this could be combined with matched filter

    rx_int = conv(rx_filt,ones(1,2*M));

    % phase and fine frequency tracking and correction ------------------------

    if gmsk_states.phase_track
 
      % DCO design from "Introduction To Phase-Lock Loop System Modeling", Wen Li
      % http://www.ece.ualberta.ca/~ee401/parts/data/PLLIntro.pdf

      eta = 0.707;
      wn = 2*pi*10*(Rs/4800);  % (Rs/4800) -> found reducing the BW benifical with falling Rs
      Ts = 1/Fs;
      g1 = 1 - exp(-2*eta*wn*Ts);
      g2 = 1 + exp(-2*eta*wn*Ts) - 2*exp(-eta*wn*Ts)*cos(wn*Ts*sqrt(1-eta*eta));
      Gpd = 2/pi;
      Gvco = 1;
      G1 = g1/(Gpd*Gvco);  G2 = g2/(Gpd*Gvco);
      %printf("g1: %e g2: %e G1: %e G2: %e\n", g1, g2, G1, G2);

      filt_prev = dco = lower = ph_err_filt = ph_err = 0;
      dco_log = filt_log = zeros(1,nsam);

      % w is the ref sine wave at the timing clock frequency
      % tw is the length of the window used to estimate timing

      k = 1;
      tw = 200*M;
      xr_log = []; xi_log = [];
      w_log = [];
      timing_clock_phase = 0;
      timing_angle = 0;
      timing_angle_log = zeros(1,nsam);

      for i=1:nsam

        % update sample timing estimate every tw samples

        if mod(i,tw) == 0
          l = i - tw+1;
          xr = abs(real(rx_int(l:l+tw-1)));
          xi = abs(imag(rx_int(l:l+tw-1)));
          w = exp(j*(l:l+tw-1)*2*pi*(Rs/2)/Fs);
          X = xr * w';
          timing_clock_phase = timing_angle = angle(X);
          k++;
          xr_log = [xr_log xr];
          xi_log = [xi_log xi];
          w_log = [w_log w];
        else
          timing_clock_phase += (2*pi)/(2*M);
        end
        timing_angle_log(i) = timing_angle;

        rx_int(i) *= exp(-j*dco);
        ph_err = sign(real(rx_int(i))*imag(rx_int(i)))*cos(timing_clock_phase);
        lower = ph_err*G2 + lower;
        filt  = ph_err*G1 + lower;
        dco   = dco + filt;
        filt_log(i) = filt;
        dco_log(i) = dco;
      end
      
      figure;
      clf
      subplot(211);
      plot(filt_log);
      title('PLL filter')
      subplot(212);
      plot(dco_log/pi);
      title('PLL DCO phase');
      %axis([1 nsam -0.5 0.5])
    end

    % sample integrator output at correct timing instant
    
    timing_adj = timing_angle_log*2*M/(2*pi);
    timing_adj_uw = unwrap(timing_angle_log)*2*M/(2*pi);
    % Toff = floor(2*M+timing_adj);
    Toff = floor(timing_adj_uw+0.5);
    k = 1;
    re_syms = im_syms = zeros(1,nsym/2);
  
    for i=2*M:2*M:nsam
      if (i-Toff(i)+M) < nsam
        re_syms(k) = real(rx_int(i-Toff(i)));
        im_syms(k) = imag(rx_int(i-Toff(i)+M));
      end
      %re_syms(k) = real(rx_int(i-10));
      %im_syms(k) = imag(rx_int(i+M-10));
      k++;
    end

    figure
    subplot(211)
    stem(re_syms)
    subplot(211)
    stem(im_syms)
    
    figure;
    clf
    subplot(211)
    plot(timing_adj);
    title('Timing est');
    subplot(212)
    plot(Toff);
    title('Timing est unwrap');

    % XORs/adders on the RHS of Muroyta et al Fig 8 (a) and (b).  We
    % simulate digital logic bit stream at clock rate Rs, even though
    % we sample integrators at rate Rs/2.  I can't explain how and why
    % this logic works/is required.  I think it can be worked out from
    % comparing to MSK/OQPSK demod designs.

    l = length(re_syms);
    l2 = 2*l;
    re_bits = zeros(1,l2);
    im_bits = zeros(1,l2);
    clk_bits = zeros(1,l2);
    for i=1:l-1
      re_bits(2*(i-1)+1)  = re_syms(i) > 0;
      re_bits(2*(i-1)+2)  = re_syms(i) > 0;
      im_bits(2*(i-1)+2)  = im_syms(i) > 0;
      im_bits(2*(i-1)+3)  = im_syms(i) > 0;
      clk_bits(2*(i-1)+1) = 0;
      clk_bits(2*(i-1)+2) = 1;
    end

    rx_bits = bitxor(bitxor(re_bits,im_bits),  clk_bits);
    rx_bits = rx_bits(2:length(rx_bits)-1);
  else
    % non-coherent demod

    % filter to get rid of most of noise before FM demod, but doesnt
    % introduce any ISI

    fc = Rs/(Fs/2);
    bin  = firls(200,[0 fc*(1-0.05) fc*(1+0.05) 1],[1 1 0.01 0.01]);
    rx_filt = filter(bin, 1, rx);

    % FM demod

    rx_diff = [ 1 rx_filt(2:nsam) .* conj(rx_filt(1:nsam-1))];
    rx_filt = (1/wd)*atan2(imag(rx_diff),real(rx_diff));

    % low pass filter, trade off betwen ISI and removing noise

    rx_filt = filter(gmsk_demod_coeff, 1, rx_filt);  
    Toff = 7;
    rx_bits = real(rx_filt(1+Toff:M:length(rx_filt)) > 0);

   end

endfunction
