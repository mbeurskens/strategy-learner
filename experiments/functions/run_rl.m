function [] = run_rl( config_file_name, rtdb, trc, refbox, ...
                       ball, simulator )
%RUN_VPG Run a vpg experiment defined in settings with name config_file_name 


config = load_experiment_config(config_file_name, '');
exp_dir = config.exp_dir;

for run = 1:config.runs
    config.exp_dir = [exp_dir '_' int2str(run)];
    if create_config_dirs(config); return, end

    if run == 1
        if init_rl(config, rtdb, trc, refbox, ball, simulator); return, end
        source = fullfile(pwd, config.exp_dir, 'init');
        target = fullfile(pwd, config.exp_dir, config.inference.model_load_dir);
        copyfile([source '/*.pt'], target);
    else
        prev_dir = [exp_dir '_' int2str(run-1)];
        source = fullfile(pwd, prev_dir, config.inference.model_save_dir);
        target = fullfile(pwd, config.exp_dir, config.inference.model_load_dir);
        copyfile([source '/*.pt'], target);

        source = fullfile(pwd, prev_dir, config.training.model_save_dir);
        target = fullfile(pwd, config.exp_dir, config.training.model_load_dir);
        copyfile([source '/*.model'], target);
    end

    %% Run experiments
    run_experiment_set(config, rtdb, trc, refbox, ball, simulator);
    train_rl(config);
    run_evaluation_set(config, rtdb, trc, refbox, ball, simulator);
    create_analysis_plots(config);

    %% Closing and saving
    source_exp_dir = strsplit(config.exp_dir, '/');
    system(['chown -R robocup ' source_exp_dir{1}]);
end

