function tx = analog_fm_mod(fm_states, mod)
Fs = fm_states.Fs;
fc = fm_states.fc;
wc = 2*pi*fc/Fs;
fd = fm_states.fd;
wd = 2*pi*fd/Fs;
nsam = length(mod);

if 1
    mod = filter(fm_states.prede,1,mod);
    mod = mod/max(mod);                                                       % AGC to set deviation
end

tx_phase = 0;
Q = zeros(1,nsam);
I = zeros(1,nsam);

for i=0:nsam-1
    w = wc + wd*mod(i+1);
    tx_phase = tx_phase + w;
    tx_phase = tx_phase - floor(tx_phase/(2*pi))*2*pi;
    tx(i+1) = exp(j*tx_phase);
    I(i+1) = real(tx(i+1));
    Q(i+1) = imag(tx(i+1));
end
I
Q

end