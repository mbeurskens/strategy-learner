%% Generate training configuration files

addpath('../../utils/jsonlab-2.0')
addpath('../../functions')
base_config = load_experiment_config('config.json', 'base');

%% Settings
learning_rates = [0.01, 0.001, 0.0001];
network_sizes = {[256, 128, 64],...
                 [128]};
algorithms = {'simple_pg', 'vpg'};

%% Generate config files in current directory
name_count = 1;
run_count = 1;
for alg_n = 1:length(algorithms)
    for lr = 1:length(learning_rates)
        for n_size = 1:length(network_sizes)
            config = base_config;
            config.rng_seed = config.rng_seed + name_count;
            name = [ num2str(name_count) '_' algorithms{alg_n}];
            config.name = name;
            config.algorithm = algorithms{alg_n};
            
            if strcmp(config.algorithm, 'simple_pg')
                config.algorithm_settings.simple_pg.learning_rate = learning_rates(lr);
                config.algorithm_settings.simple_pg.network_hidden_sizes = network_sizes{n_size};
            elseif strcmp(config.algorithm, 'vpg')
                config.algorithm_settings.vpg.policy_learning_rate = learning_rates(lr);
                config.algorithm_settings.vpg.value_learning_rate = learning_rates(lr);
                config.algorithm_settings.vpg.network_hidden_sizes = network_sizes{n_size};
            end
            
            exp_dir = fullfile(['results/thesis-1/var_' int2str(run_count)], '/run');
            config.exp_dir = exp_dir;
            savejson('', config, [config.name '.json']);
            system(['chown -R robocup ' config.name '.json']);

            name_count = name_count + 1;
            run_count = run_count + 1;
        end
    end
    name_count = 1;
end
