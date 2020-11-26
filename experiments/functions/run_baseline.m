function [] = run_baseline( config_file_name, rtdb, trc, refbox, ...
                       ball, simulator )
%RUN_BASELINE Run an experiment with the uniform sampling or rule-based 
% baseline.

config = load_experiment_config(config_file_name, '');
exp_dir = config.exp_dir;

for run = 1:config.runs
    config.exp_dir = [exp_dir '_' int2str(run)];
    if strcmp(config.algorithm, 'rule_based')
        config.inference.manual_mode = true;
    end
    if create_config_dirs(config); return, end
    
    %% Run experiments
    run_evaluation_set(config, rtdb, trc, refbox, ball, simulator);
    create_analysis_plots(config)
    
    %% Closing and saving
    source_exp_dir = strsplit(config.exp_dir, '/');
    system(['chown -R robocup ' source_exp_dir{1}]);
end
