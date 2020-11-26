/*!
 * simple_pg_model.cpp
 *
 * Load tensorflow lite simple policy gradient models and perform model inference.
 *
 *  Created on: Jun 25, 2020
 *      Author: Mickey Beurskens
 */

#include <iostream>
#include <torch/script.h>
#include "simple_pg_model.h"


SimplePGModel::SimplePGModel(const char* filename, int turtle_id): BaseModel(turtle_id)
{
    try {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        module = torch::jit::load(filename);
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model\n";
        throw;
    }
}

int SimplePGModel::RunInferenceSampled()
{
    // Execute the model and turn its output into a tensor.
    at::Tensor output = module.forward(GetModelInputTensor()).toTensor();

    // Write index of output to role shared memory as role id
    sl_model_output_t prediction;
    prediction.role = SampleCategorical(output);
    prediction.vpg_value = 0;
    mem.WritePrediction(prediction);
    return 0;
}

int SimplePGModel::RunInferenceGreedy()
{
    // Execute the model and turn its output into a tensor.
    at::Tensor output = module.forward(GetModelInputTensor()).toTensor();

    // Write index of output to role shared memory as role id
    sl_model_output_t prediction;
    prediction.role = SampleBest(output);

    prediction.vpg_value = 0;
    mem.WritePrediction(prediction);
    return 0;
}
