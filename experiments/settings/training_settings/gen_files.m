%% Generate training configuration files

addpath('../utils/jsonlab-2.0')
addpath('../functions')
base_config = load_experiment_config('config.json', 'base');

%% Settings
learning_rates = [0.001, 0.01, 0.1];
epochs = [1, 10, 100];
data_sets = {
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_1/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_2/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_3/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_4/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_5/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_6/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_7/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_8/training_data'
    '/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/gen_set_9/training_data'
};

%% Generate config files in current directory
name_count = 1;
for lr = 1:length(learning_rates)
    for ep = 1:length(epochs)
        for set = 1:length(data_sets)
            config = base_config;
            config.training.learning_rate = learning_rates(lr);
            config.training.epochs = epochs(ep);
            config.training.custom_data_path = data_sets{set};
            config.name = strcat('trained_', int2str(name_count));
            
            config_save_path = [config.name '.json'];
            savejson('', config, config_save_path);
            system(['chown -R robocup ' config.name '.json']);
            
            name_count = name_count + 1;
        end
    end
end
