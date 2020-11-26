% train.m
%
% Train algorithms using config file settings

clear classes; close all; clc;

%% Initialize
addpath('utils/jsonlab-2.0')
addpath('functions')

experiment_dir = 'settings/temp';
file_names = get_settings_file_names(experiment_dir);    
[rtdb, trc, refbox, ball, simulator] = initialize_simulator();

for exp = 1:length(file_names)
    config_file_path = fullfile(experiment_dir, file_names(exp).name);    
    run_algorithm(config_file_path, rtdb, trc, refbox, ball, simulator);
end

simulator.Stop();
