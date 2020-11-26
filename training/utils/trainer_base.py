"""
trainer_base.py
Base class for trainers that are called to train algorithms.

    date:   16-09-2020
    author: Mickey Beurskens
"""

import os
import pickle
import random
from datetime import datetime
from .import_handler import *
from abc import ABC, abstractmethod
from .meta_data import TrainingMetaDataLogger, AlgorithmMetaData


class Trainer(ABC):
    def __init__(self, settings):
        self.settings = settings
        self._update()
        self.epoch_time_array = []

    def _update(self):
        self._update_settings()
        self._init_algorithm()
        self.logger = self._init_logger()

    def _update_settings(self):
        self.name = self.settings.name
        self.epochs = self.settings.epochs
        self.source_data_dir_path = self.settings.source_data_dir_path

    def _init_logger(self):
        logger = TrainingMetaDataLogger()
        logger.set_algo_param(self._get_train_param())
        return logger

    def _save_model_checkpoint(self, epoch):
        if not self.settings.n_checkpoints == 0:
            divider = np.ceil(self.epochs/self.settings.n_checkpoints)
            if epoch % divider == 0 and epoch is not 0:
                self._save_model("checkpoint_" + str(epoch) + "_")

    def _save_final_model(self):
        self._save_model('')

    def _save_meta_data(self):
        pass

    def _save_settings(self):
        self._create_save_dir()
        self.settings.save_to_config_file()

    def _save_time_array(self):
        time_data_path = os.path.join(self.settings.save_training_model_dir,
                                      'time_per_training_epoch.timepickle')
        with open(time_data_path, 'wb') as file:
            pickle.dump(self.epoch_time_array, file)

    def _start_time(self):
        self.start_time = datetime.now()

    def _stop_time_and_add(self):
        self.epoch_time_array.append(datetime.now() - self.start_time)

    def _train_epoch(self, epoch):
        file_names = get_experiment_data_file_names(self.source_data_dir_path)
        random.shuffle(file_names)
        self._start_time()
        for episode, name in enumerate(file_names):
            self._train_episode(name, epoch)
        self._stop_time_and_add()

    def _get_max_episode_length(self):
        max_length = 0
        file_names = get_experiment_data_file_names(self.source_data_dir_path)
        for name in file_names:
            length = get_training_data_batch_length(name)
            if length > max_length:
                max_length = length
        return max_length

    def _get_action_space_size(self):
        file_names = get_experiment_data_file_names(self.source_data_dir_path)
        for name in file_names:
            return get_action_space_size(name)

    def train(self):
        for epoch in range(self.epochs):
            self._train_epoch(epoch)
            self._save_model_checkpoint(epoch)
        self._save_final_model()
        self._save_meta_data()
        # self._save_settings()

    def load_settings(self, settings: TrainingSettings):
        self.settings = settings
        self._update()

    @staticmethod
    def _create_save_dir(dir_name):
        if not os.path.exists(os.path.dirname(os.path.abspath(dir_name))):
            os.makedirs(os.path.dirname(dir_name), exist_ok=True)

    @abstractmethod
    def _init_algorithm(self):
        pass

    @abstractmethod
    def _get_train_param(self) -> AlgorithmMetaData:
        pass

    @abstractmethod
    def _train_episode(self, batch_file_name, epoch):
        pass

    @abstractmethod
    def _save_model(self, name):
        pass
