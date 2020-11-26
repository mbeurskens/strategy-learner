function [ stop_flag, config ] = generate_settings_files( config, cur_dir )
%GENERATE_SETTINGS_FILES Generate settings based on main experiment config
addpath('utils/jsonlab-2.0');
stop_flag = 0;

%% Params
gen_dir = [cur_dir '/' config.name '/training_data'];
test_dir =  [cur_dir '/' config.name '/test_data'];
alg_dir =   [cur_dir '/' config.name '/algorithm_data'];
set_dir =   [cur_dir '/' config.name '/generated_settings'];

gen_config_path = [set_dir '/generation_config.json'];
train_config_path = [set_dir '/training_config.json'];
test_config_path = [set_dir '/testing_config.json'];

%% Create experiment config files
model_path = [cur_dir '/' config.name '/algorithm_data/'];
model_name = [model_path 'algo'];
config = create_generation_config(config, gen_config_path, ...
                                  gen_dir, model_name);
config = create_training_config(config, train_config_path, ...
                                gen_dir, model_name);
config = create_testing_config(config, test_config_path, ...
                               test_dir, model_name);

%% Set config file names in config struct
config.generation.config_path = gen_config_path;
config.training.config_path = train_config_path;
config.testing.config_path = test_config_path;

end

