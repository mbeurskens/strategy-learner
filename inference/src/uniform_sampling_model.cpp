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
#include "uniform_sampling_model.h"


UniformSamplingModel::UniformSamplingModel(int turtle_id): BaseModel(turtle_id){}

int UniformSamplingModel::RunInferenceGreedy() {
    return RunInference();
}

int UniformSamplingModel::RunInferenceSampled() {
    return RunInference();
}

int UniformSamplingModel::RunInference()
{
    static const int action_space = 4; //TODO: Make action space size adaptable to current environment
    static const float uniform_action_chance = 1/(float)action_space;
    at::Tensor distribution = torch::ones({1, action_space})*uniform_action_chance;

    sl_model_output_t prediction;
    prediction.role = SampleCategorical(distribution);
    prediction.vpg_value = 0;
    mem.WritePrediction(prediction);
    return 0;
}
