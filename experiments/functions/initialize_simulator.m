function [ rtdb, trc, refbox, ball, simulator ] = initialize_simulator()
%INITIALIZE_SIMULATOR Return initialized simulator objects
%   Detailed explanation goes here
rtdb = MatlabRtdb.Rtdb();
trc = MatlabRtdb.TRC(rtdb);
refbox = MatlabRtdb.Refbox(rtdb);
ball = MatlabRtdb.Ball(rtdb);
simulator = MatlabRtdb.Simulator(rtdb, refbox);
end

