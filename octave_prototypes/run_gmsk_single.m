sim_in.precoding = 0;
sim_in.nsym = 10000;
sim_in.EbNodB = 2:10;
sim_in.verbose = 1;
nsym =  sim_in.nsym;
EbNodB = sim_in.EbNodB;
verbose = sim_in.verbose;
Rs = 4800;

gmsk_states.verbose = verbose;
gmsk_states.precoding = sim_in.precoding;
gmsk_states.phase_track = 0;
gmsk_states = gmsk_init(gmsk_states, Rs);
M = gmsk_states.M;
Fs = gmsk_states.Fs;
Rs = gmsk_states.Rs;
Bfm = gmsk_states.fm_states.Bfm;
ne =9

    aEbNodB = EbNodB(ne);
    EbNo = 10^(aEbNodB/10);
    variance = Fs/(Rs*EbNo);
    
    tx_bits = randi([0 1], 1, 1000)
    %tx_bits = ones(1, nsym);
    %tx_bits = zeros(1, nsym);
    %tx_bits(1:2:nsym) = 0;
    [tx tx_filt tx_symbols] = gmsk_mod(gmsk_states, tx_bits);
    nsam = length(tx);

    noise = sqrt(variance/2)*(randn(1,nsam) + j*randn(1,nsam));
    rx    = tx*exp(j*pi/2) + noise;

    [rx_bits rx_out rx_filt] = gmsk_demod(gmsk_states, rx(1:length(rx)));

    % search for frame location over a range

    Nerrs_min = nsym;
    Nbits_min = nsym;
    l = length(rx_bits);
    for i=1:100;
        Nerrs = sum(xor(rx_bits(i:l), tx_bits(1:l-i+1)));
        if Nerrs < Nerrs_min
            Nerrs_min = Nerrs;
            Nbits_min = l;
            Nerrs
            i
        end
    end

    TERvec = Nerrs_min;
    BERvec = Nerrs_min/Nbits_min;
    
    figure();
    plot(real(tx))
    hold on;
    plot(imag(tx))