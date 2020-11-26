"""
Code snippets based on OpenAI Spinningup: https://github.com/openai/spinningup
"""

from torch.optim import Adam
import os
import time
import random
import argparse
import vpg.core as core
from utils.import_handler import *
from utils.meta_data import AlgorithmMetaData
from utils.trainer_base import Trainer
from vpg.logger import EpochLogger
from vpg.mpi_utils import proc_id

from vpg.run_utils import setup_logger_kwargs
from vpg.mpi_pytorch import setup_pytorch_for_mpi, sync_params, mpi_avg_grads
from vpg.mpi_tools import proc_id, mpi_statistics_scalar, num_procs


class VPGBuffer:
    """
    A buffer for storing trajectories experienced by a VPG agent interacting
    with the environment, and using Generalized Advantage Estimation (GAE-Lambda)
    for calculating the advantages of state-action pairs.
    """

    def __init__(self, obs_dim, act_dim, size, gamma=0.99, lam=0.95):
        self.obs_buf = np.zeros(core.combined_shape(size, obs_dim), dtype=np.float32)
        self.act_buf = np.zeros(core.combined_shape(size, act_dim), dtype=np.float32)
        self.adv_buf = np.zeros(size, dtype=np.float32)
        self.rew_buf = np.zeros(size, dtype=np.float32)
        self.ret_buf = np.zeros(size, dtype=np.float32)
        self.val_buf = np.zeros(size, dtype=np.float32)
        self.logp_buf = np.zeros(size, dtype=np.float32)
        self.gamma, self.lam = gamma, lam
        self.ptr, self.path_start_idx, self.max_size = 0, 0, size

    def store(self, obs, act, rew, val, logp):
        """
        Append one timestep of agent-environment interaction to the buffer.
        """
        assert self.ptr < self.max_size  # buffer has to have room so you can store
        self.obs_buf[self.ptr] = obs
        self.act_buf[self.ptr] = act
        self.rew_buf[self.ptr] = rew
        self.val_buf[self.ptr] = val
        self.logp_buf[self.ptr] = logp
        self.ptr += 1

    def finish_path(self, last_val=0):
        """
        Call this at the end of a trajectory, or when one gets cut off
        by an epoch ending. This looks back in the buffer to where the
        trajectory started, and uses rewards and value estimates from
        the whole trajectory to compute advantage estimates with GAE-Lambda,
        as well as compute the rewards-to-go for each state, to use as
        the targets for the value function.
        The "last_val" argument should be 0 if the trajectory ended
        because the agent reached a terminal state (died), and otherwise
        should be V(s_T), the value function estimated for the last state.
        This allows us to bootstrap the reward-to-go calculation to account
        for timesteps beyond the arbitrary episode horizon (or epoch cutoff).
        """

        path_slice = slice(self.path_start_idx, self.ptr)
        rews = np.append(self.rew_buf[path_slice], last_val)
        vals = np.append(self.val_buf[path_slice], last_val)

        # the next two lines implement GAE-Lambda advantage calculation
        deltas = rews[:-1] + self.gamma * vals[1:] - vals[:-1]
        self.adv_buf[path_slice] = core.discount_cumsum(deltas, self.gamma * self.lam)

        # the next line computes rewards-to-go, to be targets for the value function
        self.ret_buf[path_slice] = core.discount_cumsum(rews, self.gamma)[:-1]

        self.path_start_idx = self.ptr

    def get(self):
        """
        Call this at the end of an epoch to get all of the data from
        the buffer, with advantages appropriately normalized (shifted to have
        mean zero and std one). Also, resets some pointers in the buffer.
        """
        assert self.ptr == self.max_size  # buffer has to be full before you can get
        self.ptr, self.path_start_idx = 0, 0
        # the next two lines implement the advantage normalization trick
        adv_mean, adv_std = mpi_statistics_scalar(self.adv_buf)
        if adv_std == 0:
            self.adv_buf = self.adv_buf - adv_mean
        else:
            self.adv_buf = (self.adv_buf - adv_mean) / adv_std
        data = dict(obs=self.obs_buf, act=self.act_buf, ret=self.ret_buf,
                    adv=self.adv_buf, logp=self.logp_buf)
        return {k: torch.as_tensor(v, dtype=torch.float32) for k, v in data.items()}


class VPGTrainer(Trainer):
    def __init__(self, settings):
        super().__init__(settings)
        # Special function to avoid certain slowdowns from PyTorch + MPI combo.
        setup_pytorch_for_mpi()

    def _init_algorithm(self):
        self._init_train_param()
        self._init_actor_critic()
        self._init_optimizers()

    def _init_train_param(self):
        self.train_v_iters = 80
        self.pi_lr = self.settings.algorithms["vpg"]["policy_learning_rate"]
        self.vf_lr = self.settings.algorithms["vpg"]["value_learning_rate"]
        self.hidden_sizes = self.settings.algorithms["vpg"]["network_hidden_sizes"]

    def _init_actor_critic(self):
        # Instantiate environment and create action and observation space
        self.obs_space = gym.spaces.Box(np.float(-20), np.float(20),
                                        [ModelInput.n_obs], dtype=np.float64)
        self.action_space = gym.spaces.Discrete(self._get_action_space_size())
        self.ac = core.MLPActorCritic(observation_space=self.obs_space,
                                      action_space=self.action_space,
                                      hidden_sizes=self.hidden_sizes)
        sync_params(self.ac)  # Synchronise parameters across processes
        # Attempt to load previous model
        model = os.path.join(self.settings.load_training_model_dir,
                             self.settings.algorithms
                             ['vpg']['training_model_name'])
        if os.path.exists(model):
            self.ac = torch.load(model)
            print("Loading model from: " + model)
        else:
            default_model = os.path.join(self.name,
                                         self.settings.algorithms
                                         ['vpg']['training_model_name'])
            if os.path.exists(default_model):
                self.ac = torch.load(default_model)
                print("Loading previously trained model")
            else:
                print("Cannot find model to load at filepath: '" +
                      model + "', Creating new model")

    def _init_experience_buffer(self, episode_length):
        gamma = 0.999  # gamma(float): Discount factor.(Always between 0 and 1.)
        lam = 0.997  # lam(float): Lambda for GAE - Lambda.(Always between 0 and 1, close to 1.)
        local_steps_per_epoch = int(episode_length / num_procs())
        self.buf = VPGBuffer(self.obs_space.shape, self.action_space.shape,
                             local_steps_per_epoch, gamma, lam)

    def _init_optimizers(self):
        self.pi_optimizer = Adam(self.ac.pi.parameters(), lr=self.pi_lr)
        self.vf_optimizer = Adam(self.ac.v.parameters(), lr=self.vf_lr)
        # TODO: Integrate two learning rates in settings file

    def _init_logger(self):
        logger_kwargs = setup_logger_kwargs(self.name, 0)
        logger_kwargs['output_dir'] =\
            os.path.join(self.settings.save_training_model_dir, 'log')
        logger = EpochLogger(**logger_kwargs)
        logger.save_config(locals())
        var_counts = tuple(core.count_vars(module) for module in [self.ac.pi,
                                                                  self.ac.v])
        logger.log('\nNumber of parameters: \t pi: %d, \t v: %d\n'
                   % var_counts)
        logger.setup_pytorch_saver(self.ac)
        self.total_interactions = 0
        return logger

    def _get_train_param(self) -> AlgorithmMetaData:
        pass

    def _train_episode(self, batch_file_name, epoch):
        batch = get_vpg_training_data_batch(batch_file_name)
        ep_ret = 0
        ep_len = len(batch)
        self._init_experience_buffer(ep_len)
        for step in batch:
            o, a, r, v = step
            logp = self.ac.get_logp_a(torch.tensor([o]), torch.tensor(a))
            ep_ret += r
            self.logger.store(VVals=v)
            self.buf.store(o, a, r, v, logp)

        self.logger.store(EpRet=ep_ret, EpLen=ep_len)
        self.buf.finish_path(batch[-1][3])
        self._update_vpg()

    def _train_epoch(self, epoch):
        file_names = get_experiment_data_file_names(self.source_data_dir_path)
        random.shuffle(file_names)
        start_time = time.time()

        self._start_time()
        for episode, name in enumerate(file_names):
            self._train_episode(name, epoch)
        self._stop_time_and_add()
        self._log_one_epoch(epoch, start_time)

    def _log_one_epoch(self, epoch, start_time):
        self.logger.log_tabular('Epoch', epoch)
        self.logger.log_tabular('EpRet', with_min_and_max=True)
        self.logger.log_tabular('EpLen', average_only=True)
        self.logger.log_tabular('VVals', with_min_and_max=True)
        self.logger.log_tabular('LossPi', average_only=True)
        self.logger.log_tabular('LossV', average_only=True)
        self.logger.log_tabular('DeltaLossPi', average_only=True)
        self.logger.log_tabular('DeltaLossV', average_only=True)
        self.logger.log_tabular('Entropy', average_only=True)
        self.logger.log_tabular('KL', average_only=True)
        self.logger.log_tabular('Time', time.time() - start_time)
        self.logger.dump_tabular()

    def _save_meta_data(self):
        pass

    def _update_vpg(self):
        data = self.buf.get()

        # Get loss and info values before update
        pi_l_old, pi_info_old = self._compute_loss_pi(data)
        pi_l_old = pi_l_old.item()
        v_l_old = self._compute_loss_v(data).item()

        # Train policy with a single step of gradient descent
        self.pi_optimizer.zero_grad()
        loss_pi, pi_info = self._compute_loss_pi(data)
        loss_pi.backward()
        mpi_avg_grads(self.ac.pi)  # average grads across MPI processes
        self.pi_optimizer.step()

        # Value function learning
        for i in range(self.train_v_iters):
            self.vf_optimizer.zero_grad()
            loss_v = self._compute_loss_v(data)
            loss_v.backward()
            mpi_avg_grads(self.ac.v)  # average grads across MPI processes
            self.vf_optimizer.step()

        self._log_update(pi_info, pi_info_old, pi_l_old, v_l_old,
                         loss_pi, loss_v)

    def _log_update(self, pi_info, pi_info_old, pi_l_old,
                    v_l_old, loss_pi, loss_v):
        kl, ent = pi_info['kl'], pi_info_old['ent']
        self.logger.store(LossPi=pi_l_old, LossV=v_l_old,
                          KL=kl, Entropy=ent,
                          DeltaLossPi=(loss_pi.item() - pi_l_old),
                          DeltaLossV=(loss_v.item() - v_l_old))

    def _compute_loss_pi(self, data):
        obs, act, adv, logp_old = data['obs'], data['act'], data['adv'], data['logp']

        # Policy loss
        pi, logp = self.ac.pi(obs, act)
        loss_pi = -(logp * adv).mean()

        # Useful extra info
        approx_kl = (logp_old - logp).mean().item()
        ent = pi.entropy().mean().item()
        pi_info = dict(kl=approx_kl, ent=ent)
        return loss_pi, pi_info

    def _compute_loss_v(self, data):
        obs, ret = data['obs'], data['ret']
        return ((self.ac.v(obs) - ret) ** 2).mean()

    def _save_model(self, name):
        self._save_time_array()
        self._create_save_dir(self.settings.save_inference_model_dir)
        self._create_save_dir(self.settings.save_training_model_dir)
        example_obs = torch.rand(1, 20)
        policy_model = torch.jit.trace(self.ac.pi.logits_net, example_obs)
        value_model = torch.jit.trace(self.ac.v.v_net, example_obs)
        policy_model.save(os.path.join(self.settings.save_inference_model_dir,
                                       name + self.settings.algorithms['vpg']
                                       ['policy_inference_model_name']))
        value_model.save(os.path.join(self.settings.save_inference_model_dir,
                                      name + self.settings.algorithms['vpg']
                                      ['value_inference_model_name']))
        torch.save(self.ac, os.path.join(self.settings.save_training_model_dir,
                                         name + self.settings.algorithms['vpg']
                                         ['training_model_name']))

    def init_model(self):
        self._save_model('')


if __name__ == '__main__':
    # TODO: Add multi processing back in
    #mpi_fork(args.cpu)  # run parallel code with mpi
    parser = argparse.ArgumentParser(description='Run vanilla policy '
                                                 'gradient training.')
    parser.add_argument('-s', dest='settings_path', action='store', type=str,
                        help='path to settings file')
    parser.add_argument('-i', dest='init', action='store', type=bool,
                        help='Initialize a non trained network and store it')
    args = parser.parse_args()

    if args.settings_path:
        vpg_settings = TrainingSettings(args.settings_path)
    else:
        vpg_settings = TrainingSettings("../experiments/results/vpg/run_1/init/init_train_conf.json")

    trainer = VPGTrainer(vpg_settings)
    if not args.init:
        trainer.train()
    else:
        trainer.init_model()

