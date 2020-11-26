"""
meta_data.py
Data structures storing meta data generated during training.
"""

import os
import pickle
import numpy as np
from typing import List
from datetime import datetime


class AlgorithmMetaData:
    def __init__(self):
        self.name: str = ""
        self.learning_rate: float = 0
        self.network_layer_sizes: List[int] = []
        self.network_layer_types: List[str] = []


class EpisodeMetaData:
    def __init__(self, idx, frames, loss, rewards, source_file_path):
        self.idx: int = idx
        self.frames: int = frames
        self.loss: float = loss
        self.rewards: List[float] = rewards
        self.source_file_path: str = source_file_path


class EpochMetaData:
    def __init__(self, idx):
        self.idx: int = idx
        self.mean_loss: float = 0.0
        self.mean_reward: float = 0.0
        self.mean_frames: int = 0
        self.episodes: List[EpisodeMetaData] = []
        self.start_time: datetime.now = None
        self.training_time: datetime.now = None

    def start_timer(self):
        self.start_time = datetime.now()

    def stop_timer(self):
        self.training_time = datetime.now() - self.start_time

    def add_episode(self, episode: EpisodeMetaData):
        self.episodes.append(episode)
        self._update_mean()

    def _update_mean(self):
        self.mean_loss = self._get_mean_loss()
        self.mean_reward = self._get_mean_reward()
        self.mean_frames = self._get_mean_frames()

    def _get_mean_loss(self):
        return np.mean([float(episode.loss) for episode in self.episodes])

    def _get_mean_reward(self):
        return np.mean([np.sum(episode.rewards) for episode in self.episodes])

    def _get_mean_frames(self):
        return np.mean([episode.frames for episode in self.episodes])


class TrainingMetaData:
    def __init__(self):
        self.idx: int = 0
        self.date: int = 0
        self.epochs: List[EpochMetaData] = []
        self.algorithm: AlgorithmMetaData = None


class TrainingMetaDataLogger:
    def __init__(self):
        self.training_md: TrainingMetaData = TrainingMetaData()
        cur_date = datetime.now().strftime("%Y%m%d%H%M%S")
        self.training_md.date = cur_date
        self.training_md.idx = int(cur_date)

    def start_epoch(self):
        self._add_epoch()
        self._start_epoch_timer()

    def stop_epoch(self):
        self._stop_epoch_timer()

    def add_episode(self, frames: int, loss: float, 
                    rewards: List[float], source_file_path: str):
        if not self.training_md.epochs:
            self._add_epoch()
        self._update_epoch(frames, loss, rewards, source_file_path)
            
    def set_algo_param(self, algo_meta: AlgorithmMetaData):
        self.training_md.algorithm = algo_meta

    def print_epoch_data(self):
        print('epoch: %3d \t mean_loss: %.3f \t return: %.3f \t ep_len: %.3f '
              '\t train_time: %s' %
              (self.training_md.epochs[-1].idx,
               self.training_md.epochs[-1].mean_loss,
               self.training_md.epochs[-1].mean_reward,
               len(self.training_md.epochs[-1].episodes),
               self.training_md.epochs[-1].training_time))

    def save(self, save_path):
        with open(save_path, 'wb') as file:
            pickle.dump(self.training_md, file)

    def _start_epoch_timer(self):
        self.training_md.epochs[-1].start_timer()
    
    def _stop_epoch_timer(self):
        if self.training_md.epochs[-1]:
            self.training_md.epochs[-1].stop_timer()

    def _add_epoch(self):
        self.training_md.epochs.append(self._get_new_epoch())

    def _update_epoch(self, frames, loss, rewards, source_file_path):
        self.training_md.epochs[-1].add_episode(
            self._get_new_episode(frames, loss, rewards, source_file_path)
        )

    def _get_new_episode(self, frames, loss, rewards, source_file_path):
        new_episode_idx = len(self.training_md.epochs[-1].episodes)
        return EpisodeMetaData(new_episode_idx, frames, loss,
                               rewards, source_file_path)

    def _get_new_epoch(self):
        new_epoch_idx = len(self.training_md.epochs)
        return EpochMetaData(new_epoch_idx)


if __name__ == "__main__":
    tmd_logger = TrainingMetaDataLogger()
    tmd_logger.start_epoch()
    tmd_logger.add_episode(10, 10, [1, 2, 3], "source_dir")
    tmd_logger.stop_epoch()
    tmd_logger.print_epoch_data()

