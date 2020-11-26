function [ stop ] = create_config_dirs( config )
%CREATE_CONFIG_DIRS Create directory structure based on config file
%   Return stop if user stops creation using prompt
stop = 0;

training_model_save_dir = fullfile(pwd, config.exp_dir, config.training.model_save_dir);
training_model_load_dir = fullfile(pwd, config.exp_dir, config.training.model_load_dir);
training_data_load_dir = fullfile(pwd, config.exp_dir, config.training.data_load_dir);
training_config_file_dir = fullfile(pwd, config.exp_dir, config.config_dir, config.training.config_file_dir);

inference_model_save_dir = fullfile(pwd, config.exp_dir, config.inference.model_save_dir);
inference_model_load_dir = fullfile(pwd, config.exp_dir, config.inference.model_load_dir);
inference_data_save_dir = fullfile(pwd, config.exp_dir, config.inference.data_save_dir);
inference_config_file_dir = fullfile(pwd, config.exp_dir, config.config_dir, config.inference.config_file_dir);

evaluation_data_dir = fullfile(pwd, config.exp_dir, 'evaluation');

%% Create file directory structure
command = ['mkdir -p ' config.exp_dir ' ' ...
    training_model_save_dir ' ' ...
    training_model_load_dir ' ' ...
    training_data_load_dir ' ' ...
    training_config_file_dir ' ' ...
    inference_model_save_dir ' ' ...
    inference_model_load_dir ' ' ...
    inference_data_save_dir ' ' ...
    inference_config_file_dir ' ' ...
    evaluation_data_dir];

if (system(command) ~= 0)
    prompt = ['The experiment directory or a subdirectory already ' ...
        'exist, continue anyway? (y/n): '];
    user_input = input(prompt, 's');
    if (strcmp(user_input, 'y') == 0)
        stop = 1;
        return;
    end
end

%% Copy config to folder for future reference
config_save_path = fullfile(pwd, config.exp_dir, config.config_dir, 'config.json');
savejson('', config, config_save_path);
source_exp_dir = strsplit(config.exp_dir, '/');
system(['chown -R robocup ' source_exp_dir{1}]);
end

