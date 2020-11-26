function [] = run_experiment_set( config, rtdb, trc, refbox, ...
                                  ball, simulator )
%RUN_EXPERIMENT_SET Run a set of experiments defined in experiment_config

config.evaluation.greedy_mode = false;
generate_inference_settings(config);
config_file_path = fullfile(pwd, config.exp_dir, config.config_dir,  ...
                            config.inference.config_file_dir, ...
                            config.inference.config_file_name);
set_sim_settings(config_file_path);
set_permissions(config);
for experiment_n = 1:config.inference.episodes
    fprintf('Current run episode: %d/%d\n', experiment_n, ...
            config.inference.episodes);

    run_experiment(config.inference.episode_time, rtdb, ...
                   config.turtles, ball, simulator, ...
                   refbox, trc);
end

end

