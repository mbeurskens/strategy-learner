function [ stop_flag, config ] = load_settings(experiment_dir, config_file_name)
%LOAD_SETTINGS Load settings and return a config struct
config = load_experiment_config(config_file_name, experiment_dir);
cur_dir = pwd;
[stop_flag, config] = generate_settings_files(config, cur_dir);

end

