//
// Created by Mickey Beurskens on 20-08-20.
//

#ifndef INFERENCE_CONFIG_LOADER_H
#define INFERENCE_CONFIG_LOADER_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ConfigLoader {
private:
    json config;

public:
    ConfigLoader();

    void LoadConfigFile(const char* file_path);

    const char* GetModelFilePath();

    const char* GetVPGPolicyFilePath();

    const char* GetVPGValueFilePath();

    const char* GetTurtleDataSavePath();

    const char* GetAlgorithm();

    bool GetManualTurtleBehaviourFlag();

    bool GetGreedyModeFlag();
};


#endif //INFERENCE_CONFIG_LOADER_H
