import os
import numpy as np
import pandas as pd
from typing import List
from matplotlib import pyplot as plt
from evaluation import get_experiments_data


def save_figure(fig, save_dir, file_name):
    if not os.path.exists(save_dir):
        os.makedirs(save_dir)
    fig.savefig(os.path.join(save_dir, file_name + ".png"), format="png")


def save_plots(exp_dirs: List[str], turtle_ids: List[int], save_dir: str):
    data = get_experiments_data(exp_dirs)
    save_mean_epoch_reward_plot(data, turtle_ids, save_dir)
    save_mean_epoch_passes_plot(data, turtle_ids, save_dir)
    save_mean_epoch_passes_box(data, turtle_ids, save_dir)
    save_time_simulating_training_plot(data, turtle_ids, save_dir)
    save_time_spent_computing_algorithm_training_plot(data, turtle_ids, save_dir)
    save_time_spent_training_plot(data, turtle_ids, save_dir)
    save_training_frames_simulated_plot(data, turtle_ids, save_dir)
    save_action_distribution_plot(data, turtle_ids, save_dir)
    save_composite_time_plot(data, [turtle_ids[0]], save_dir)


def save_mean_epoch_reward_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                                save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                axes.plot(data["run"], data["mean epoch reward"])
                labels.append(name + ' turtle ' + str(int(t_id)))
    axes.legend(labels)
    axes.set_title("Mean epoch reward")
    axes.set_xlabel("run [-]")
    axes.set_ylabel("mean reward [-]")
    axes.set_ylim([0, 15])
    save_figure(fig, save_dir, "mean_epoch_reward")


def save_mean_epoch_passes_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                                save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                axes.plot(data["run"], data["mean epoch passes"])
                labels.append(name + ' turtle ' + str(int(t_id)))
    axes.legend(labels)
    axes.set_title("Mean epoch passes")
    axes.set_xlabel("run [-]")
    axes.set_ylabel("mean passes [-]")
    axes.set_ylim([0, 15])
    save_figure(fig, save_dir, "mean_epoch_passes")


def save_mean_epoch_passes_box(exp_data: pd.DataFrame, turtle_ids: List[int],
                               save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    box_data = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                box_data.append(list(data["mean epoch passes"]))
                labels.append(name + ' turtle ' + str(int(t_id)))
    axes.boxplot(box_data, labels=labels)
    axes.set_title("Mean epoch passes")
    axes.set_ylabel("mean passes [-]")
    axes.set_ylim([0, 15])
    save_figure(fig, save_dir, "mean_epoch_passes_box")


def save_training_frames_simulated_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                                        save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                axes.plot(data["run"], data["simulated training frames over runs"])
                labels.append(name + ' turtle ' + str(int(t_id)))
    axes.legend(labels)
    axes.set_title("Training frames over runs")
    axes.set_xlabel("run [-]")
    axes.set_ylabel("frames [-]")
    save_figure(fig, save_dir, "training_frames_simulated")


def save_time_simulating_training_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                                       save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                axes.plot(data["run"], data["simulated training time over runs"])
                labels.append(name + ' turtle ' + str(int(t_id)))
    axes.legend(labels)
    axes.set_title("Time spent simulating training")
    axes.set_xlabel("run [-]")
    axes.set_ylabel("time [s]")
    save_figure(fig, save_dir, "time_spent_simulating_training")


def save_time_spent_computing_algorithm_training_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                                                      save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                axes.plot(data["run"], data["computation time over runs"])
                labels.append(name + ' turtle ' + str(int(t_id)))
    axes.legend(labels)
    axes.set_title("Time spent computing algorithm training")
    axes.set_xlabel("run [-]")
    axes.set_ylabel("computing time [s]")
    save_figure(fig, save_dir, "time_spent_computing_algorithm_training")


def save_time_spent_training_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                                  save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                axes.plot(data["run"], data["training time over runs"])
                labels.append(name + ' turtle ' + str(int(t_id)))
    axes.legend(labels)
    axes.set_title("Time spent training")
    axes.set_xlabel("run [-]")
    axes.set_ylabel("training time [s]")
    save_figure(fig, save_dir, "time_spent_training")


def save_composite_time_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                             save_dir: str):
    fig, axes = plt.subplots(1)
    labels = []
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                for key in ['simulated training time over runs',
                            'computation time over runs',
                            'training time over runs']:
                    axes.plot(data["run"], data[key])
                    labels.append(name + ' ' + key + ' turtle ' + str(int(t_id)))
    axes.legend(labels)
    axes.set_title("Training time overview")
    axes.set_xlabel("run [-]")
    axes.set_ylabel("training time [s]")
    save_figure(fig, save_dir, "composite_time")


def save_action_distribution_plot(exp_data: pd.DataFrame, turtle_ids: List[int],
                                  save_dir: str):
    width = 0.15
    for t_id in turtle_ids:
        if t_id in exp_data["turtle"]:
            for name in exp_data["name"].unique():
                fig, axes = plt.subplots(1)
                data = exp_data[exp_data["turtle"] == t_id]
                data = data[data["name"] == name].sort_values(by="run")
                axes.plot(data["run"], data["action pass"], label="pass")
                axes.plot(data["run"], data["action receive"], label="receive")
                axes.plot(data["run"], data["action move"], label="move")
                axes.plot(data["run"], data["action intercept"], label="intercept")

                axes.set_title("Action frequency distribution overview " + name + "_t_" + str(int(t_id)))
                axes.legend()
                axes.set_xlabel("run [-]")
                axes.set_ylabel("action frequency [-]")
                axes.set_xticks(data["run"])
                axes.set_ylim([0, 1])
                save_figure(fig, save_dir, "action_distribution_" + name + "_t_" + str(int(t_id)))
