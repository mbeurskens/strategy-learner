import glob
import torch
import json
from .structs import *
from typing import List
from .reader import DataReader


def load_time_synched_data_set(file_paths):
    """ Return time synchronised pandas data frames for turtle data in
    file_paths. """
    data_turtles = []
    for path in file_paths:
        data_turtles.append(DataReader.read_data(path))
    pre_proc = PreProcessor(data_turtles)
    return pre_proc.get_time_synchronized_turtle_data()


def get_experiment_data_file_names(dir_path):
    """ Load all experiment data file names from a directory and return data in
     a list ."""
    return glob.glob(dir_path + "/model_data*")


def _unpack_input(data: ModelTrainingData):
    """ Return flat model input array """
    return [data.model_input.own_xyo_x, data.model_input.own_xyo_y,
            data.model_input.own_xyo_o, data.model_input.own_xyo_dot_x,
            data.model_input.own_xyo_dot_y, data.model_input.own_xyo_dot_o,
            data.model_input.peer_xyo_x, data.model_input.peer_xyo_y,
            data.model_input.peer_xyo_o, data.model_input.peer_xyo_dot_x,
            data.model_input.peer_xyo_dot_y, data.model_input.peer_xyo_dot_o,
            data.model_input.ball_xyz_x, data.model_input.ball_xyz_y,
            data.model_input.ball_xyz_z, data.model_input.ball_xyz_dot_x,
            data.model_input.ball_xyz_dot_y, data.model_input.ball_xyz_dot_z,
            data.model_input.own_ball_possession,
            data.model_input.peer_ball_possession]


def _unpack_reward_data(data: ModelTrainingData):
    return float(data.env_data.reward)


def _unpack_output(data: ModelTrainingData):
    """ Return flat model output array. """
    return [data.model_output.role, data.model_output.vpg_value]


def _unpack_role(data: ModelTrainingData):
    return data.model_output.role


def _unpack_vpg_value(data: ModelTrainingData):
    return data.model_output.vpg_value


def unpack_vpg_batch(data: list):
    unpacked_data = []
    for step in data:
        unpacked_data.append([
            _unpack_input(step),
            _unpack_role(step),
            _unpack_reward_data(step),
            _unpack_vpg_value(step),
        ])
    return unpacked_data


def get_vpg_training_data_batch(file_name):
    experiment_data = DataReader.read_data(file_name)
    return unpack_vpg_batch(experiment_data)


def unpack_simple_pg_batch(data: list):
    data_obs = []
    data_roles = []
    data_rewards = []
    for step in data:
        data_obs.append(_unpack_input(step))
        data_roles.append(_unpack_role(step))
        data_rewards.append(_unpack_reward_data(step))
    return data_obs, data_roles, data_rewards


def get_simple_pg_training_data_batch(file_name):
    experiment_data = DataReader.read_data(file_name)
    return unpack_simple_pg_batch(experiment_data)


def get_training_data_batch_length(file_name):
    return len(DataReader.read_data(file_name))


def get_action_space_size(file_name):
    return DataReader.read_data(file_name)[0].model_output.action_space_size


class TrainingSettings:
    def __init__(self, settings_path):
        self.load_from_config_file(settings_path)

    def save_to_config_file(self):
        file_name = self.name + "_settings.json"
        with open(file_name, "w") as file:
            json.dump(vars(self), file)

    def load_from_config_file(self, file_name):
        with open(file_name, "r") as file:
            self._set_variables_from_dict(json.load(file))

    def _set_variables_from_dict(self, dictionary):
        for key, variable in dictionary.items():
            setattr(self, key, variable)