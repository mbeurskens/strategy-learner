function [stop] = init_rl(config, rtdb, trc, refbox, ball, simulator)
%INIT_RL Initialize model for training in python
%   If model type is not found return stop
stop = 0;
local_conf = config;
init_dir = fullfile('init');

%% Create init directory
command = ['mkdir -p ' fullfile(pwd, config.exp_dir, init_dir)];
system(command);

%% Initialization training settings
local_conf.config_dir = '';
local_conf.training.epochs = 1;
local_conf.training.n_model_save_checkpoints = 0;
local_conf.training.data_load_dir = init_dir;
local_conf.training.config_file_dir = init_dir;
local_conf.training.config_file_name = 'init_train_conf.json';

%% Initialization inference settings
local_conf.inference.manual_mode = true;
local_conf.inference.episode_time = 1;
local_conf.inference.episodes = 1;
local_conf.inference.data_save_dir = init_dir;
local_conf.inference.model_load_dir = init_dir;
local_conf.inference.model_save_dir = init_dir;
local_conf.inference.config_file_dir = init_dir;
local_conf.inference.config_file_name = 'init_inference_conf.json';

%% Perform manual inference and initialize model
% Run manual mode to get example training data (somewhat of a hack)
rng(local_conf.rng_seed, 'Twister')
run_experiment_set(local_conf, rtdb, trc, refbox, ball, simulator);
set_permissions(local_conf);
generate_training_settings(local_conf);
settings_path = fullfile(pwd, local_conf.exp_dir, ...
                         local_conf.training.config_file_dir, ...
                         local_conf.training.config_file_name);

if strcmp(config.algorithm, 'simple_pg')
    command = ['/home/robocup/svn/trunk/src/Turtle2/Strategy/src/' ...
    'STP/strategy_learner/venv/bin/python3.6 /home/robocup/svn/trunk/' ...
    'src/Turtle2/Strategy/src/STP/' ...
    'strategy_learner/training/train_simple_pg.py -s ' settings_path ...
    ' -i True'];
    system(command);
elseif strcmp(config.algorithm, 'vpg')
    command = ['/home/robocup/svn/trunk/src/Turtle2/Strategy/src/' ...
    'STP/strategy_learner/venv/bin/python3.6 /home/robocup/svn/trunk/' ...
    'src/Turtle2/Strategy/src/STP/' ...
    'strategy_learner/training/train_vpg.py -s ' settings_path ...
    ' -i True'];
    system(command);
else
    fprintf('Algorithm "%s" not found for initialization. Exiting...\n', ...
        config.algorithm);
    stop = 1;
end

%% Remove init data dir
init_data_path = fullfile(pwd, local_conf.exp_dir, ...
                          local_conf.inference.data_save_dir);
system(['rm ' init_data_path '/*.mb']);

end

