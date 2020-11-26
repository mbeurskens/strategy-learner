function [config] = create_training_config( config, destination_path, ...
                                            data_path, model_name )
%CREATE_ALGO_CONFIG Create settings file for algorithm training
gen_config.name = model_name;
gen_config.epochs = config.training.epochs;
gen_config.learning_rate = config.training.learning_rate;
gen_config.source_data_dir_path = data_path;
gen_config.network_hidden_sizes = config.training.network_hidden_sizes;
gen_config.n_checkpoints = config.training.n_model_save_checkpoints;
gen_config.load_model_path = config.training.load_model_path;

savejson('', gen_config, destination_path);
end
