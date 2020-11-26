function [] = train_rl(config)
%TRAIN_RL Train a policy gradient algorithm in python
generate_training_settings(config);
set_permissions(config)
settings_path = fullfile(pwd, config.exp_dir, config.config_dir, ...
                         config.training.config_file_dir, ...
                         config.training.config_file_name);
if strcmp(config.algorithm, 'simple_pg')
    command = ['/home/robocup/svn/trunk/src/Turtle2/Strategy/src/' ...
    'STP/strategy_learner/venv/bin/python3.6 /home/robocup/svn/trunk/' ...
    'src/Turtle2/Strategy/src/STP/' ...
    'strategy_learner/training/train_simple_pg.py -s ' settings_path];
elseif strcmp(config.algorithm, 'vpg')
    command = ['/home/robocup/svn/trunk/src/Turtle2/Strategy/src/' ...
    'STP/strategy_learner/venv/bin/python3.6 /home/robocup/svn/trunk/' ...
    'src/Turtle2/Strategy/src/STP/' ...
    'strategy_learner/training/train_vpg.py -s ' settings_path];
end

system(command);
end