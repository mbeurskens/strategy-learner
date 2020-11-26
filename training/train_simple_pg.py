"""
train_simple_pg.py
A simple policy gradient built in pytorch.

    date:   30-06-2020
    author: Mickey Beurskens
"""

import os
import argparse
from torch.optim import Adam
from torch.distributions.categorical import Categorical
from utils.meta_data import AlgorithmMetaData
from utils.import_handler import *
from utils.trainer_base import Trainer


def mlp(sizes, activation=torch.nn.Tanh, output_activation=torch.nn.Identity):
    """ Build a multi layer perceptron network. """
    layers = []
    for j in range(len(sizes) - 1):
        act = activation if j < len(sizes) - 2 else output_activation
        layers += [torch.nn.Linear(sizes[j], sizes[j + 1]), act()]
        if j == len(sizes) - 2:
            layers += [torch.nn.Softmax(dim=1)]
    return torch.nn.Sequential(*layers)


class PGTrainer(Trainer):
    def __init__(self, settings):
        super().__init__(settings)

    def _init_algorithm(self):
        self.input_dim = ModelInput.n_obs
        self.output_dim = self._get_action_space_size()
        self._init_simple_pg()
        self.optimizer = self._init_optimizer()

    def _init_simple_pg(self):
        self.network_sizes = None
        model = os.path.join(self.settings.load_training_model_dir,
                             self.settings.algorithms
                             ['simple_pg']['training_model_name'])
        if os.path.exists(model):
            self.network = torch.load(model)
            print("Loading model from: " + model)
        else:
            default_model = os.path.join(self.name,
                                         self.settings.algorithms
                                         ['simple_pg']['training_model_name'])
            if os.path.exists(default_model):
                self.network = torch.load(default_model)
                print("Loading previously trained model")
            else:
                print("Cannot find model to load at filepath: '" +
                      model + "', Creating new model")
                if isinstance(self.settings.algorithms['simple_pg']['network_hidden_sizes'], List):
                    hid_size = self.settings.algorithms['simple_pg']['network_hidden_sizes']
                else:
                    hid_size = [self.settings.algorithms['simple_pg']['network_hidden_sizes']]
                self.network_sizes = [self.input_dim] + hid_size + \
                                     [self.output_dim]
                self.network = mlp(sizes=self.network_sizes)

    def _init_network_sizes(self):
        return [self.input_dim] + \
               self.settings.algorithms['simple_pg']['network_hidden_sizes'] + \
               [self.output_dim]

    def _init_optimizer(self):
        return Adam(self.network.parameters(),
                    lr=self.settings.algorithms['simple_pg']['learning_rate'])

    def _get_policy(self, obs):
        # Compute action distribution
        logits = self.network(obs)
        return Categorical(logits=logits)

    def _get_action(self, obs):
        # Action selection function sampled from policy
        return self._get_policy(obs).sample().item()

    def _compute_loss(self, obs, act, weights):
        # Loss function whose gradient is policy gradient
        log_p = self._get_policy(obs).log_prob(act)
        return -(log_p * weights).mean()

    def _get_train_param(self):
        algo_meta = AlgorithmMetaData()
        algo_meta.name = self.name
        algo_meta.learning_rate = \
            self.settings.algorithms['simple_pg']['learning_rate']
        if self.network_sizes:
            algo_meta.network_layer_sizes = self.network_sizes
            algo_meta.network_layer_types = \
                ["in"] + ["mlp"] * len(self.network_sizes) + ["out"]
        return algo_meta

    def _save_model(self, name):
        self._save_time_array()
        self._create_save_dir(self.settings.save_inference_model_dir)
        self._create_save_dir(self.settings.save_training_model_dir)
        example_obs = torch.rand(1, 20)
        inference_file_path = os.path.join(self.settings.save_inference_model_dir,
                                           name + self.settings.algorithms
                                           ['simple_pg']['inference_model_name'])

        training_file_path = os.path.join(self.settings.save_training_model_dir,
                                          name + self.settings.algorithms
                                          ['simple_pg']['training_model_name'])
        torch_script_model = torch.jit.trace(self.network, example_obs)
        torch_script_model.save(inference_file_path)
        torch.save(self.network, training_file_path)

    def _train_episode(self, batch_file_name, epoch):
        batch = get_simple_pg_training_data_batch(batch_file_name)
        batch_obs = torch.as_tensor(batch[0], dtype=torch.float32)
        batch_acts = torch.as_tensor(batch[1], dtype=torch.int32)
        batch_rewards = batch[2]

        ep_rews = batch_rewards
        ep_ret, ep_len = sum(ep_rews), len(ep_rews)
        # for R(tau) weighting in policy gradient
        batch_weights = torch.as_tensor([ep_ret] * ep_len, dtype=torch.float32)

        # take a single policy gradient update step
        self.optimizer.zero_grad()
        batch_loss = self._compute_loss(obs=batch_obs, act=batch_acts,
                                        weights=batch_weights)
        batch_loss.backward()
        self.optimizer.step()
        self.logger.add_episode(0, batch_loss, batch_rewards, batch_file_name)
        return batch_loss, batch_rewards

    def _train_epoch(self, epoch):
        self.logger.start_epoch()
        self._start_time()
        super()._train_epoch(epoch)
        self._stop_time_and_add()
        self.logger.stop_epoch()
        self.logger.print_epoch_data()

    def init_model(self):
        self._save_model('')


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run simple policy '
                                                 'gradient training.')
    parser.add_argument('-s', dest='settings_path', action='store', type=str,
                        help='path to settings file')
    parser.add_argument('-i', dest='init', action='store', type=bool,
                        help='Initialize a non trained network and store it')
    args = parser.parse_args()

    if args.settings_path:
        simple_pg_settings = TrainingSettings(args.settings_path)
    else:
        simple_pg_settings = TrainingSettings("/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/results/test/simple_pg/run_1/config/generated/training.json")

    trainer = PGTrainer(simple_pg_settings)
    if not args.init:
        trainer.train()
    else:
        trainer.init_model()
