function [ file_list ] = get_settings_file_names( settings_dir )
%GET_SETTINGS_FILE_NAMES Get all settings file in settings directory

file_list = dir([settings_dir '/*.json']);
if length(file_list) == 1 
    file_list = [file_list];
end
file_list = orderfields(file_list);
end

