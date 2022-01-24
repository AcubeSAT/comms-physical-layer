function fm_states = analog_fm_in(fm_states)

% FM modulator constants

Fs = fm_states.Fs;
FsOn2 = Fs/2;
fm_max = fm_states.fm_max;                                          % max modulation freq
fd = fm_states.fd;                                                  % (max) deviation
fm_states.m = fd/fm_max;
Bfm = 2*(fd+fm_max); 
% modulation index
fm_states.Bfm = Bfm;       
                        % Carson's rule for FM signal bandwidth
tc = 50E-6;
fm_states.tc = tc;
fm_states.prede = [1 -(1 - 1/(tc*Fs))];                   % pre/de emp filter coeffs
fm_states.ph_dont_limit = 0;                                   % Limit rx delta-phase

% Select length of filter to be an integer number of symbols to
% assist with "fine" timing offset estimation.  Set Ts to 1 for
% analog modulation.

Ts = fm_states.Ts;
desired_ncoeffs = 200;
ncoeffs = floor(desired_ncoeffs/Ts+1)*Ts;

% "coarse" timing offset is half filter length, we have two filters.
% This is the delay the two filters introduce, so we need to adjust
% for this when comparing tx to trx bits for BER calcs.

fm_states.nsym_delay = ncoeffs/Ts;

% input filter gets rid of excess noise before demodulator, as too much
% noise causes atan2() to jump around, e.g. -pi to pi.  However this
% filter can cause harmonic distortion at very high SNRs, as it knocks out
% some of the FM signal spectra.  This filter isn't really required for high
% SNRs > 20dB.

fc = (Bfm/2)/(FsOn2);
fm_states.bin  = firls(ncoeffs,[0 fc*(1-0.05) fc*(1+0.05) 1],[1 1 0.01 0.01]);

% demoduator output filter to limit us to fm_max (e.g. 3kHz)

fc = fm_max/(FsOn2);
fm_states.bout = firls(ncoeffs,[0 0.95*fc 1.05*fc 1], [1 1 0.01 0.01]);
end
