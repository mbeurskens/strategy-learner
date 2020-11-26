/*!
 * vpg_model.cpp
 *
 * Load tensorflow lite vanilla policy gradient models and perform model inference.
 *
 *  Created on: Sep 17, 2020
 *      Author: Mickey Beurskens
 */

#include <iostream>
#include <torch/script.h>
#include "vpg_model.h"


VPGModel::VPGModel(const char* policy_model_file_path, const char* value_model_file_path, int turtle_id):
    BaseModel(turtle_id)
{
    try {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        policy_module = torch::jit::load(policy_model_file_path);
        value_model = torch::jit::load(value_model_file_path);
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model\n";
        throw;
    }
}

int VPGModel::RunInferenceSampled()
{
    // Execute the model and turn its output into a tensor.
    at::Tensor policy_output = policy_module.forward(GetModelInputTensor()).toTensor();
    at::Tensor value_output = value_model.forward(GetModelInputTensor()).toTensor();
    std::cout << value_output << std::endl;

    sl_model_output_t prediction;
    prediction.role = SampleCategorical(policy_output);
    prediction.vpg_value = value_output.item().toFloat();
    mem.WritePrediction(prediction);
    return 0;
}

int VPGModel::RunInferenceGreedy()
{
    // Execute the model and turn its output into a tensor.
    at::Tensor policy_output = policy_module.forward(GetModelInputTensor()).toTensor();
    at::Tensor value_output = value_model.forward(GetModelInputTensor()).toTensor();

    sl_model_output_t prediction;
    prediction.role = SampleBest(policy_output);
    prediction.vpg_value = value_output.item().toFloat();
    mem.WritePrediction(prediction);
    return 0;
}
