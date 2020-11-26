/*!
 * turtle.cpp
 *
 * Manage shared memory connection and inference for each turtle on a separate thread.
 *
 *  Created on: Jul 16, 2020
 *      Author: Mickey Beurskens
 */

#include <fstream>
#include "timer.h"
#include "turtle.h"
#include "vpg_model.h"
#include "simple_pg_model.h"
#include "uniform_sampling_model.h"
#include "info_shared_memory.h"

Turtle::Turtle(int turtle_id): t_id(turtle_id), joined(false), left(false), finish(false), model_load_error(false) {}

Turtle::Turtle(const Turtle &t, int turtle_id): t_id(turtle_id), joined(false), left(false), finish(false),
    model_load_error(false) {}

void Turtle::Run(bool &exit_flag, const char* settings_file_path) {
    std::cout << "\tStarting thread for turtle " << t_id << "\n";
    // Start an info server to check whether turtle shared memory is online
    InfoShm info_server(t_id, false);
    bool local_exit_flag = false;

    std::cout << "\tWaiting for turtle " << t_id << " to start in the TRC\n";
    while (!exit_flag && !local_exit_flag && !finish) {
        if (info_server.GetTurtleRunningFlag()) {
            ConnectToTurtle(exit_flag, local_exit_flag, info_server, settings_file_path);
        }
    }
    std::cout << "\tShutting down thread for turtle " << t_id << "\n";
}

turtle_info Turtle::LoadTurtleInfo(const char* settings_file_path) {
    configs.LoadConfigFile(settings_file_path);
    turtle_info t_info;
    t_info.manual_mode_flag = (int)configs.GetManualTurtleBehaviourFlag();
    strcpy(t_info.turtle_data_save_dir, configs.GetTurtleDataSavePath());
    t_info.running_flag = 1;
    model_path = configs.GetModelFilePath();
    vpg_policy_model_path = configs.GetVPGPolicyFilePath();
    vpg_value_model_path = configs.GetVPGValueFilePath();
    algorithm = configs.GetAlgorithm();
    greedy_mode_flag = configs.GetGreedyModeFlag();
    return t_info;
}

void Turtle::ConnectToTurtle(bool &exit_flag, bool &local_exit_flag,
                             InfoShm &info_server, const char* settings_file_path) {
    turtle_info t_info = LoadTurtleInfo(settings_file_path);
    info_server.Write(t_info);
    std::cout << "\tTurtle " << t_id << " started " << "\n";
    joined = true;
    if (info_server.Read().manual_mode_flag) {
        RunManualMode(exit_flag, info_server);
    } else {
        if (!ModelPathValid()) {
            model_load_error = true;
        } else {
            RunLearnedMode(exit_flag, info_server);
        }
    }
    local_exit_flag = true;
    left = true;
}

void Turtle::RunLearnedMode(bool &exit_flag, InfoShm &info_server) {
    Timer time(1000);
    int milli_sec = 500;
    Timer connect_time(milli_sec);
    connect_time.Sleep();
    std::cout << "\tFound turtle " << t_id << "\n\tLoading inference model: " << algorithm
              << " for turtle " << t_id << "\n";

    if(strcmp(algorithm, "simple_pg") == 0) {
        SimplePGModel model(model_path, t_id);
        RunInferenceModel(exit_flag, info_server, model, time);
    } else if(strcmp(algorithm, "vpg") == 0) {
        VPGModel model(vpg_policy_model_path, vpg_value_model_path, t_id);
        RunInferenceModel(exit_flag, info_server, model, time);
    } else if(strcmp(algorithm, "uniform_sampling") == 0) {
        UniformSamplingModel model(t_id);
        RunInferenceModel(exit_flag, info_server, model, time);
    } else {
        std::cout << "\tWARNING: Could not find model for algorithm: " << algorithm << ". Loading BaseModel.\n";
        BaseModel model(t_id);
        RunInferenceModel(exit_flag, info_server, model, time);
    }
}

void Turtle::RunManualMode(bool &exit_flag, InfoShm &info_server) {
    Timer time(10);
    time.Reset();
    while (!exit_flag && info_server.GetTurtleRunningFlag()) {
        time.Sleep();
    }
}

void Turtle::RunInferenceModel(bool &exit_flag, InfoShm &info_server, BaseModel &model, Timer &time) {
    time.Reset();
    if(greedy_mode_flag){
        while (!exit_flag && info_server.GetTurtleRunningFlag()) {
            model.RunInferenceGreedy();
            time.Sleep();
        }
    } else {
        while (!exit_flag && info_server.GetTurtleRunningFlag()) {
            model.RunInferenceSampled();
            time.Sleep();
        }
    }
}

bool Turtle::GetJoined() {
    return joined;
}

bool Turtle::GetLeft() {
    return left;
}

bool Turtle::GetLoadError() {
    return model_load_error;
}

void Turtle::SetFinish(bool flag) {
    finish = flag;
}

bool Turtle::ModelPathValid() {
    if(strcmp(algorithm, "simple_pg") == 0) {
        std::ifstream f(model_path);
        if (!f.good()){
            std::cout << "\tModel path: " << model_path << " for turtle " << t_id << " not valid " << t_id << "\n";
        }
        return f.good();
    } else if(strcmp(algorithm, "vpg") == 0) {
        std::ifstream pi(vpg_policy_model_path);
        std::ifstream v(vpg_value_model_path);
        if(pi.good() && v.good()) {
            return true;
        }
        std::cout << "\tModel path: " << vpg_policy_model_path << "\n\t or " << vpg_value_model_path <<
                  " for turtle " << t_id << " not valid " << t_id << "\n";
        return false;
    } else if(strcmp(algorithm, "uniform_sampling") == 0) {
        return true;
    }
    std::cout << "\tError: Turtle model type not valid when searching for model path" << std::endl;
    return false;
}