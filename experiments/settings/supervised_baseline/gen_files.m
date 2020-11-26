%% Generate training configuration files

addpath('../../utils/jsonlab-2.0')
addpath('../../functions')
base_config = load_experiment_config('config.json', 'base');

%% Settings
learning_rates = [0.001, 0.0001, 0.00001];
network_sizes = {[256, 128, 64],...
                 [128, 64],...
                 [128]};

%% Generate config files in current directory
name_count = 1;
for lr = 1:length(learning_rates)
    for n_size = 1:length(network_sizes)
        config = base_config;
        config.rng_seed = config.rng_seed + name_count;
        name = ['sup_var_' num2str(name_count)];
        config.name = name;
        config.algorithm_settings.simple_pg.learning_rate = learning_rates(lr);
        config.algorithm_settings.simple_pg.network_hidden_sizes = network_sizes{n_size};

        exp_dir = fullfile(['results/supervised_slow/var_' int2str(name_count)], '/run');
        config.exp_dir = exp_dir;
        savejson('', config, [config.name '.json']);
        system(['chown -R robocup ' config.name '.json']);

        name_count = name_count + 1;
    end
end
