function [] = generate_inference_settings( config )
%GENERATE_INFERENCE_SETTINGS Generate inference settings file
%   Generate a inference config file for an experiment run

exp_dir = fullfile(pwd, config.exp_dir);
gen_config.algorithm = config.algorithm;
gen_config.turtle_manual_mode = config.inference.manual_mode;
gen_config.greedy_mode_flag = config.evaluation.greedy_mode;

gen_config.turtle_data_save_path = fullfile(exp_dir, config.inference.data_save_dir);
gen_config.model_file_path = fullfile(exp_dir, config.inference.model_load_dir, ...
                                      config.algorithm_settings.simple_pg.inference_model_name);
gen_config.vpg_policy_model_file_path = fullfile(exp_dir, config.inference.model_load_dir, ...
                                                 config.algorithm_settings.vpg.policy_inference_model_name);
gen_config.vpg_value_model_file_path = fullfile(exp_dir, config.inference.model_load_dir, ...
                                                config.algorithm_settings.vpg.value_inference_model_name);

savejson('', gen_config, fullfile(exp_dir, config.config_dir, ...
                         config.inference.config_file_dir, ...
                         config.inference.config_file_name));

end

