//
// Created by Mickey Beurskens on 20-08-20.
//

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "config_loader.h"

using json = nlohmann::json;
ConfigLoader::ConfigLoader() {}

void ConfigLoader::LoadConfigFile(const char* file_path) {
    std::ifstream config_stream(file_path);
    config = json::parse(config_stream);
}

const char* ConfigLoader::GetModelFilePath() {
    std::string name = config["model_file_path"].get<std::string>();
    return name.c_str();
}

const char* ConfigLoader::GetVPGPolicyFilePath() {
    std::string name = config["vpg_policy_model_file_path"].get<std::string>();
    return name.c_str();
}

const char* ConfigLoader::GetVPGValueFilePath() {
    std::string name = config["vpg_value_model_file_path"].get<std::string>();
    return name.c_str();
}

const char* ConfigLoader::GetTurtleDataSavePath() {
    std::string name = config["turtle_data_save_path"].get<std::string>();
    return name.c_str();
}

const char* ConfigLoader::GetAlgorithm() {
    std::string name = config["algorithm"].get<std::string>();
    return name.c_str();
}

bool ConfigLoader::GetManualTurtleBehaviourFlag() {
    bool flag = config["turtle_manual_mode"].get<bool>();
    return flag;
}

bool ConfigLoader::GetGreedyModeFlag() {
    bool flag = config["greedy_mode_flag"].get<bool>();
    return flag;
}
