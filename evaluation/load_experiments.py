import os
import glob
import json
import pickle
import numpy as np
import pandas as pd
from typing import List
from training.utils.reader import DataReader, ModelTrainingData


def get_run_number(run_dir_path: str) -> int:
    return int(run_dir_path.split("_")[-1])


def get_episode_reward(episode_data: List[ModelTrainingData]):
    return np.sum([step.env_data.reward for step in episode_data])


def get_episode_passes(episode_data: List[ModelTrainingData]):
    return np.sum([step.env_data.n_balls_passed for step in episode_data])


def get_action_distribution(run_data_list: List[List[ModelTrainingData]]):
    total_samples = 0
    dist = np.zeros(run_data_list[0][0].model_output.action_space_size)
    for d in run_data_list:
        total_samples += len(d)
        for t_step in d:
            dist[t_step.model_output.role] += 1
    dist = dist/total_samples
    return dist


def total_times_over_runs(times: pd.DataFrame, key: str) -> pd.DataFrame:
    totals = pd.DataFrame()
    times = times.reset_index(drop=True)
    for i, pd_index in enumerate(times.index):
        totals = totals.append([times[key][:i].sum() + times[key][pd_index]], ignore_index=True)
    times[key + " over runs"] = totals
    return times


def get_time_spent_simulating_training_per_run(exp_data: pd.DataFrame):
    return total_times_over_runs(exp_data["simulated training data time per epoch"])


def get_total_time_over_runs(exp_data: pd.DataFrame, key: str) -> pd.DataFrame:
    new_data = pd.DataFrame()
    for t_id in exp_data["turtle"].unique():
        for name in exp_data["name"].unique():
            set_with_comp_data = exp_data[(exp_data['name'] == name) & (exp_data['turtle'] == t_id)]
            set_with_comp_data = total_times_over_runs(set_with_comp_data, key)
            new_data = new_data.append(set_with_comp_data, ignore_index=True)
    return new_data


def get_run_turtle_data(data_dir_path: str, turtle_id: int) -> dict:
    run_data = []
    reader = DataReader()
    search_string = data_dir_path + "/model_data*t" + str(turtle_id) + "*.mb"
    files = glob.glob(search_string)
    if len(files) > 0:
        for index, file in enumerate(files):
            run_data.append(reader.read_data(file))
    action_distribution = get_action_distribution(run_data)

    return {"mean epoch reward": np.mean([get_episode_reward(episode) for episode in run_data]),
            "quantile 1st epoch reward": np.quantile([get_episode_reward(episode) for episode in run_data], 0.25),
            "quantile 3rd epoch reward": np.quantile([get_episode_reward(episode) for episode in run_data], 0.75),
            "mean epoch passes": np.mean([get_episode_passes(episode) for episode in run_data]),
            "quantile 1st epoch passes": np.quantile([get_episode_passes(episode) for episode in run_data], 0.25),
            "quantile 3rd epoch passes": np.quantile([get_episode_passes(episode) for episode in run_data], 0.75),
            "action pass": action_distribution[0],
            "action receive": action_distribution[1],
            "action move": action_distribution[2],
            "action intercept": action_distribution[3]}


def get_computation_time(run_dir_path: str) -> dict:
    file_path = os.path.join(run_dir_path,
                             "models/training/time_per_training_epoch.timepickle")
    if os.path.exists(file_path):
        with open(file_path, "rb") as file:
            epoch_training_times = pickle.load(file)
        return {"computation time": np.sum([np.round(time.microseconds)/1e6 for time in epoch_training_times])}
    return {"computation time": 0}


def get_run_meta_data(run_dir_path: str) -> dict:
    with open(os.path.join(run_dir_path, "config/config.json"), 'r') as file:
        meta_data_file: dict = json.load(file)
    simulated_epoch_time_for_training = meta_data_file["inference"]["episodes"] * meta_data_file["inference"]["episode_time"]
    meta_data = {"name": meta_data_file["name"],
                 "algorithm": meta_data_file["algorithm"],
                 "evaluation episodes per epoch": meta_data_file["evaluation"]["episodes"],
                 "evaluation time per episode": meta_data_file["evaluation"]["episode_time"],
                 "training data episodes per epoch": meta_data_file["inference"]["episodes"],
                 "training data time per episode": meta_data_file["inference"]["episode_time"],
                 "simulated training time": simulated_epoch_time_for_training,
                 "simulated training frames": round(simulated_epoch_time_for_training*100)}

    if meta_data_file["algorithm"] == "simple_pg":
        meta_data["learning rate"] = meta_data_file["algorithm_settings"]["simple_pg"]["learning_rate"]
        meta_data["network sizes"] = meta_data_file["algorithm_settings"]["simple_pg"]["network_hidden_sizes"]
    elif meta_data_file["algorithm"] == "vpg":
        meta_data["learning rate"] = meta_data_file["algorithm_settings"]["vpg"]["policy_learning_rate"]
        meta_data["network sizes"] = meta_data_file["algorithm_settings"]["vpg"]["network_hidden_sizes"]
    elif meta_data_file["algorithm"] == "rule_based" or meta_data_file["algorithm"] == "uniform_sampling":
        meta_data["learning rate"] = None
        meta_data["network sizes"] = None
        meta_data["training data time per episode"] = 0
        meta_data["simulated training time"] = 0
    return meta_data


def get_run_data(run_dir_path: str, turtle_id: int) -> dict:
    run_data_dict = {"run": get_run_number(run_dir_path),
                     "turtle": turtle_id,
                     "training time": None}
    run_data_dict.update(get_run_meta_data(run_dir_path))
    run_data_dict.update(get_run_turtle_data(os.path.join(run_dir_path, "evaluation"), turtle_id))
    run_data_dict.update(get_computation_time(run_dir_path))
    return run_data_dict


def get_experiment_data(experiment_dir_path: str) -> pd.DataFrame:
    experiment_data = pd.DataFrame()
    for t_id in [2, 3]:
        dirs = [d for d in os.listdir(experiment_dir_path)
                if os.path.isdir(os.path.join(experiment_dir_path, d))]
        for run_dir in dirs:
            if not run_dir == "figures":
                run_data = get_run_data(
                    os.path.join(experiment_dir_path, run_dir), t_id)
                experiment_data = experiment_data.append(run_data, ignore_index=True)
    experiment_data = get_total_time_over_runs(
        experiment_data.sort_values(by=["name", "turtle", "run"]), 'computation time')
    experiment_data = get_total_time_over_runs(
        experiment_data.sort_values(by=["name", "turtle", "run"]), 'simulated training time')
    experiment_data = get_total_time_over_runs(
        experiment_data.sort_values(by=["name", "turtle", "run"]), 'simulated training frames')
    experiment_data["training time"] = experiment_data["computation time"] \
                                       + experiment_data["simulated training time"]
    experiment_data["training time over runs"] = experiment_data["computation time over runs"] \
                                                 + experiment_data["simulated training time over runs"]
    return experiment_data


def get_experiments_data(experiment_dir_paths: List[str]) -> pd.DataFrame:
    experiment_data = pd.DataFrame()
    for dir_path in experiment_dir_paths:
        experiment_data = experiment_data.append(get_experiment_data(dir_path))
    return experiment_data


if __name__ == "__main__":
    test = get_experiment_data("/home/robocup/svn/trunk/src/Turtle2/Strategy/src/STP/strategy_learner/experiments/results/11-nov-spg-1")
    print(test)
