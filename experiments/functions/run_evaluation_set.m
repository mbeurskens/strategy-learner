function [] = run_evaluation_set( config, rtdb, trc, refbox, ...
                                  ball, simulator )
%RUN_EVALUATION_SET Run a set of evaluations defined in experiment_config

config.inference.data_save_dir = 'evaluation';
config.evaluation.greedy_mode = true;
generate_inference_settings(config);
config_file_path = fullfile(pwd, config.exp_dir, config.config_dir,  ...
                            config.inference.config_file_dir, ...
                            config.inference.config_file_name);
set_sim_settings(config_file_path);
set_permissions(config);
for experiment_n = 1:config.evaluation.episodes
    fprintf('Current evaluation run episode: %d/%d\n', experiment_n, ...
            config.evaluation.episodes);

    run_experiment(config.evaluation.episode_time, rtdb, ...
                   config.turtles, ball, simulator, ...
                   refbox, trc);
end

end

