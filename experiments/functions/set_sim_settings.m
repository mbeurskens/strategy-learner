function [] = set_sim_settings(config_file_path)
%SET_SIM_SETTINGS Copy file in config file path to inference settings.
copyfile(config_file_path, '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/inference/settings/inference_settings.json')
end

