"""
structs.py
Data structures related to strategy learner.

author:   		Mickey Beurskens <mickey_beurskens@hotmail.com>
created:     	2020-April-16
"""

import gym
import numpy as np
from ctypes import *
from gym import spaces
from .helper import check_type_in_list

SL_TASK_PASS = 0
SL_TASK_RECEIVE = 1
SL_TASK_MOVE = 2
SL_TASK_INTERCEPT = 3


class ModelInput(Structure):
    """
    Model input data structure.
    """
    _fields_ = [('own_xyo_x', c_double), ('own_xyo_y', c_double),
                ('own_xyo_o', c_double), ('own_xyo_dot_x', c_double),
                ('own_xyo_dot_y', c_double), ('own_xyo_dot_o', c_double),
                ('peer_xyo_x', c_double), ('peer_xyo_y', c_double),
                ('peer_xyo_o', c_double), ('peer_xyo_dot_x', c_double),
                ('peer_xyo_dot_y', c_double), ('peer_xyo_dot_o', c_double),
                ('ball_xyz_x', c_double), ('ball_xyz_y', c_double),
                ('ball_xyz_z', c_double),  ('ball_xyz_dot_x', c_double),
                ('ball_xyz_dot_y', c_double), ('ball_xyz_dot_z', c_double),
                ('own_ball_possession', c_double),
                ('peer_ball_possession', c_double)]
    n_obs = len(_fields_)


class EnvironmentData(Structure):
    """
    Environment data structure.
    """
    _fields_ = [('reward', c_float), ('n_balls_passed', c_int)]


class ModelOutput(Structure):
    """
    Model output data structure.
    """
    _fields_ = [('role', c_int), ('action_space_size', c_int),
                ('vpg_value', c_float)]
    n_prediction = len(_fields_)


class ModelTrainingData(Structure):
    """
    Data to train the ML model.
    """
    _pack_ = 1
    _fields_ = [('time_stamp', c_int64), ('model_input', ModelInput),
                ('env_data', EnvironmentData), ('model_output', ModelOutput)]


class StrategyLearnerEnv(gym.Env):
    """Custom Environment that interfaces with the strategy learner in the
      Tech United MSL software.
    """
    metadata = {'render.modes': ['human']}

    def __init__(self):
        super(StrategyLearnerEnv, self).__init__()
        # Define action and observation space

        # They must be gym.spaces objects
        # Example when using discrete actions:
        self.action_space = spaces.Discrete(ModelOutput.n_actions)

        # Example for using image as input:
        self.observation_space = spaces.Box(low=0, high=1,
                                            shape=[ModelInput.n_obs],
                                            dtype=np.double)

    def step(self, action):
        # Execute one time step within the environment
        return None

    def reset(self):
        # Reset the state of the environment to an initial state
        return None

    def render(self, mode='human', close=False):
        # Render the environment to the screen
        return None


class PreProcessor:
    def __init__(self, structures: list):
        """
        Pre-process model training data to make it suitable for plotting.
        :param structures: Time ordered list of ModelTrainingData structures
            for a single turtle.
        """
        self._structures = structures

    def process_new_data(self, structures: list):
        """
        Process new turtle data using the pre processor.
        :param structures: Time ordered lists of ModelTrainingData structures
            for a single turtle.
        """
        self.__init__(structures)

    def get_time_synchronized_turtle_data(self):
        """
        Return time synchronized turtle data.
        :return: Time synchronized list of turtle data.
        """
        return self._synchronise_time_data(self._structures)

    def get_role_distributions(self):
        distributions = []
        for struct in self._structures:
            dist = np.array([0, 0, 0, 0])
            for t in struct:
                if t.model_output.role == SL_TASK_PASS:
                    dist[0] += 1
                elif t.model_output.role == SL_TASK_RECEIVE:
                    dist[1] += 1
                elif t.model_output.role == SL_TASK_MOVE:
                    dist[2] += 1
                elif t.model_output.role == SL_TASK_INTERCEPT:
                    dist[3] += 1
            distributions.append(dist/dist.sum())
        return distributions

    def _synchronise_time_data(self, structures: list):
        """
        Prune structures to cover the same time span.
        :param structures: List of ModelTrainingData structures.
        """
        [t_min, t_max] = self._get_time_bounds(structures)
        for struct in structures:
            check_type_in_list(struct, ModelTrainingData)
            # Find indexes corresponding to bounds
            min_index, max_index = self._get_time_bound_indexes(struct,
                                                                t_min, t_max)
            # Delete data outside of bounds
            del struct[0:min_index]
            del struct[(max_index-min_index):]
        return structures

    @staticmethod
    def _get_time_bound_indexes(struct, min_time, max_time):
        """
        Return indexes for struct corresponding to time bounds and normalize
            time.
        :param struct: Time ordered list of ModelTrainingData structures
            for a single turtle.
        :param min_time: Lower time bound for cut-off.
        :param max_time: Upper time bound for cut-off.
        :return:
        """
        min_found = False
        max_found = False
        for step in range(len(struct)):
            if struct[step].time_stamp >= min_time and not min_found:
                min_index = step
                min_found = True
            if struct[step].time_stamp >= max_time and not max_found:
                max_index = step
                max_found = True
            struct[step].time_stamp = struct[step].time_stamp - min_time
        if not max_found:
            max_index = step
        if min_found and max_found:
            return min_index, max_index
        else:
            return 0, step

    @staticmethod
    def _get_time_bounds(structures: list):
        """
        Return the time span shared between data structures.
        :param structures: List of data structs for each turtle.
        :return: [min_time, max_time]: Shared time bounds.
        """
        min_time = structures[0][0].time_stamp
        max_time = structures[0][-1].time_stamp
        for struct in structures:
            if min_time < struct[0].time_stamp:
                min_time = struct[0].time_stamp
            if max_time > struct[-1].time_stamp:
                max_time = struct[-1].time_stamp
        return [min_time, max_time]


if __name__ == "__main__":
    sl_env = StrategyLearnerEnv()
