function [] = generate_training_settings(config)
%GENERATE_TRAINING_SETTINGS Generate training settings file
%   Generate a training config file for a training run

gen_config.name = config.name;
gen_config.epochs = config.training.epochs;
gen_config.source_data_dir_path = fullfile(pwd, config.exp_dir, ...
                                       config.training.data_load_dir);


gen_config.n_checkpoints = config.training.n_model_save_checkpoints;


gen_config.load_training_model_dir = fullfile(pwd, config.exp_dir, ...
                                      config.training.model_load_dir);
gen_config.save_training_model_dir = fullfile(pwd, config.exp_dir, ...
                                      config.training.model_save_dir);
gen_config.save_inference_model_dir = fullfile(pwd, config.exp_dir, ...
                                      config.inference.model_save_dir);
                                       
gen_config.algorithms.simple_pg = config.algorithm_settings.simple_pg;
gen_config.algorithms.vpg =  config.algorithm_settings.vpg;

savejson('', gen_config, fullfile(pwd, config.exp_dir, ...
                                  config.config_dir, ...
                                  config.training.config_file_dir, ...
                                  config.training.config_file_name));

end

