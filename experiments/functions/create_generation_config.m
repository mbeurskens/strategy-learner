function [config] = create_generation_config( config, destination_path, ...
                                       data_path, model_name )
%CREATE_GENERATION_CONFIG Create settings file for data generation
gen_config.turtle_manual_mode = config.generation.turtle_manual_mode;
gen_config.turtle_data_save_path = data_path;
gen_config.algorithm = config.algorithm;
config.generation.data_save_path = data_path;
gen_config.model_file_path = [model_name '_model.pt'];
gen_config.vpg_policy_model_file_path = '';
gen_config.vpg_value_model_file_path = '';

if strcmp(config.algorithm, 'vpg')
    if ~isempty(config.generation.load_model_dir)
        gen_config.vpg_policy_model_file_path = [config.generation.load_model_dir '/algo_vpg_policy_model.pt'];
        gen_config.vpg_value_model_file_path = [config.generation.load_model_dir '/algo_vpg_value_model.pt'];
    else
        gen_config.vpg_policy_model_file_path = [model_name '_vpg_policy_model.pt'];
        gen_config.vpg_value_model_file_path = [model_name '_vpg_value_model.pt'];
    end
end
   
savejson('', gen_config, destination_path);

end
