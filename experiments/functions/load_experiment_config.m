function [ config ] = load_experiment_config( file_name, file_dir )
%LOAD_EXPERIMENT_CONFIG Load json configuration file and return config

if isempty(file_dir) 
    config = loadjson(file_name);
else
    config = loadjson(strcat(file_dir,'/',file_name));
end
    
end

