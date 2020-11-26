function [ stop_flag ] = run_algorithm( config_file_name, rtdb, trc, ...
                                        refbox, ball, simulator )
%TRAIN_ALGORITHM Run one of the available algorithms
stop_flag = 0;
config = load_experiment_config(config_file_name, '');
if strcmp(config.algorithm, 'simple_pg') || strcmp(config.algorithm, 'vpg') 
    run_rl(config_file_name, rtdb, trc, refbox, ...
                       ball, simulator);
elseif strcmp(config.algorithm, 'rule_based') || ...
        strcmp(config.algorithm, 'uniform_sampling')
    run_baseline(config_file_name, rtdb, trc, refbox, ...
                       ball, simulator);
else
    fprintf('Algorithm "%s" not found for training. Exiting...\n', ...
        config.algorithm);
    stop_flag = 1;
end
end

