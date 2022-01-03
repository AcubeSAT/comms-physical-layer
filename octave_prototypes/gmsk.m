% gmsk.m
% David Rowe Dec 2014
%
% GMSK modem implementation and simulations to test

%
% [X] plot eye diagram
% [X] BER curves with reas match to theoretical
% [X] fine timing estimator
%     [X] test with fine timing error by resampling
% [X] phase/freq estimator
%     + need initial acquisition and tracking
%     [X] test with different freq offsets
% [X] coarse timing estimator (sync up to known test frames)
%     [X] test with different coarse timing offsets
% [ ] file read/write interface
%     [ ] refactor into tx/rx functions
% [X] modify for 1200 (or any) bit/s operation
%     + ie GMSK filter coeff generation
%     + or just re-sampling? e.g. ratio of Fs to Rs?
% [ ] way to measure input SNR to demod
%     + Maybe based on test tone/carrier from the other side?
%     + think about process ... total signal plus noise power?  Increase power until S+N doubles?
% [X] generate curves for baseline modem and with sync algorithms
%     [X] used coarse sync code to remove need for knowing delays
% [X] demod level indep
%     + scaled OK +/- 20dB same BER
% [X] effect of DC signals from SDRs
%     + simulated effect of general interferer at -1500Hz, at an amplitude of 4
%       (12 dB above GMSK signal), it started to affect BER e.g. 0.007 to 0.009
%       Line appeared about 30dB above top of GMSK signal.
% [ ] effect of quantisation noise
