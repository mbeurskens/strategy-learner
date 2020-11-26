/*!
 * base_model.h
 *
 * Base class used to load tensorflow gradient lite models and perform model inference.
 *
 *  Created on: Sep 17, 2020
 *      Author: Mickey Beurskens
 */

#include <torch/script.h>
#include "base_model.h"
#include "turtle_shared_memory.h"


BaseModel::BaseModel(int turtle_id):
    mem(turtle_id, true)
{
    t_id = turtle_id;
}

std::vector<torch::jit::IValue> BaseModel::GetModelInputTensor()
{
    // Create a vector of inputs and return
    std::vector<torch::jit::IValue> inputs;
    sl_model_input_t observation = mem.ReadObs();
    torch::Tensor in_tensor = torch::tensor({{observation.own_xyo_x, observation.own_xyo_y,
                                              observation.own_xyo_o, observation.own_xyo_dot_x,
                                              observation.own_xyo_dot_y, observation.own_xyo_dot_o,
                                              observation.peer_xyo_x, observation.peer_xyo_y,
                                              observation.peer_xyo_o, observation.peer_xyo_dot_x,
                                              observation.peer_xyo_dot_y, observation.peer_xyo_dot_o,
                                              observation.ball_xyz_x, observation.ball_xyz_y,
                                              observation.ball_xyz_z, observation.ball_xyz_dot_x,
                                              observation.ball_xyz_dot_y, observation.ball_xyz_dot_z,
                                              observation.own_ball_possession, observation.peer_ball_possession}});

    inputs.push_back(in_tensor);
    return inputs;
}

int BaseModel::RunInferenceSampled() {}

int BaseModel::RunInferenceGreedy() {}

int BaseModel::SampleCategorical(at::Tensor distribution)
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    float random = dist(e2);
    float total = 0;
    for(int i = 0; i < distribution[0].size(0); i++){
        total += distribution[0][i].item<float>();
        if(random <= total){
            return i;
        }
    }
    return 0;
}

int BaseModel::SampleBest(at::Tensor distribution)
{
    return distribution.argmax().item().to<int>();
}

BaseModel::~BaseModel() {}