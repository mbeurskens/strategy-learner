function [] = run_experiment( time, rtdb, turtle_ids, ball, simulator, refbox, trc )
%RUN_EXPERIMENT Run a single experiment for TIME amount of time.

%% Initialize turtles
for i = 1 : length(turtle_ids)
    % Start turtle
    t_id = turtle_ids(i);   % Current turtle ID
    turtles{i} = MatlabRtdb.Turtle(rtdb, t_id, MatlabRtdb.TeamColor.Magenta);
end
pause(1)

%% Random ball and turtle placement
for i = 1 : length(turtle_ids)
    turtles{i}.pose = [get_rand(-6, 6), get_rand(-8, 8), get_rand(-5, 5)];
end
ball.position = [get_rand(-6, 6), get_rand(-5, 5)];
pause(1)

%% Start playing and wait, then stop playing
refbox.command = MatlabRtdb.RefboxCommand.Start;
pause(time)
refbox.command = MatlabRtdb.RefboxCommand.Stop;

%% Close turtles
for i = 1 : length(turtles)
    turtles{i}.Stop();
end
pause(1)
