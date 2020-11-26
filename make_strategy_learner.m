%% Make all c source files in strategy learner directory
function make_strategy_learner()
curdir = cd;
p = mfilename('fullpath');
path = p(1:end-length(mfilename));

cd(path)

%% Parameters
library_name = 'strategy_learner';
compilation_data = [
    assemble_compilation_data('src', [ '-I', buspath, ' ', ...
    get_strategy_learner_includes(), ' ', ...
    get_strategy_includes(), ' -I/usr/include/libxml2' ]);
    ];
external_objs = '../../mu_positioning_lib/Mu_function_definitions.o ';

%% Check input arguments
verbosity = 'verbose';
action = 'make';
if nargin > 0
    if ~isempty(strfind(input_args, 'nonverbose'))
        verbosity = 'nonverbose';
    end
    if ~isempty(strfind(input_args, 'clean'))
        action = 'clean';
    end
end

%% Perform requested action
if exist(['lib', library_name ,'.a'], 'file') == 2
     clean_library(library_name, compilation_data, verbosity);
end
    

switch action
    case 'make'
        fprintf('Creating %s library...\n', library_name);
        make_library(library_name, compilation_data, external_objs, verbosity);
        if strcmp(verbosity, 'verbose')
            fprintf('Successfully created %s library!\n', library_name);
        else
            fprintf('\b done!\n');
        end
    case 'clean'
        fprintf('Removing %s library...\n', library_name);
        clean_library(library_name, compilation_data, verbosity);
        if strcmp(verbosity, 'verbose')
            fprintf('Successfully removed %s library!\n', library_name);
        else
            fprintf('\b done!\n');
        end
    otherwise
        error('Unknown action requested!');
end

cd(curdir)
end
