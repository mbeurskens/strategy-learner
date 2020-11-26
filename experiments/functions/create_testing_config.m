function [config] = create_testing_config( config, destination_path, ...
                                           data_path, model_name )
%CREATE_TESTING_CONFIG Create settings file for testing
gen_config.turtle_manual_mode = config.testing.turtle_manual_mode;
gen_config.turtle_data_save_path = data_path;
gen_config.algorithm = config.algorithm;
config.testing.data_save_path = data_path;
gen_config.model_file_path = [model_name '_model.pt'];
gen_config.vpg_policy_model_file_path = '';
gen_config.vpg_value_model_file_path = '';

if strcmp(config.algorithm, 'vpg')
    gen_config.vpg_policy_model_file_path = [model_name '_vpg_policy_model.pt'];
    gen_config.vpg_value_model_file_path = [model_name '_vpg_value_model.pt'];
end

savejson('', gen_config, destination_path);

end
