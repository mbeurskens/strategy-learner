function [] = create_analysis_plots( config )
%CREATE_ANALYSIS_PLOTS Create meta data of experiment run and plots from 
% source_dir downwards. Source_dir is a relative file path.

source_dir = strsplit(config.exp_dir, '/');
if length(source_dir) > 1
    dir = fullfile(source_dir{1:end-1});
else
    dir = source_dir{1};
end

generate_plots_command = ['/home/robocup/svn/trunk/src/Turtle2/Strategy/src/' ...
'STP/strategy_learner/venv/bin/python3.6 /home/robocup/svn/trunk/' ...
'src/Turtle2/Strategy/src/STP/' ...
'strategy_learner/make_plots.py -e ' ...
fullfile(pwd, dir)];

system(generate_plots_command);

end
