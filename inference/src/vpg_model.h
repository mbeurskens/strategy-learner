/*!
 * vpg_model.h
 *
 * Load tensorflow vanilla policy gradient lite models and perform model inference.
 *
 *  Created on: Sep 17, 2020
 *      Author: Mickey Beurskens
 */

#ifndef INFERENCE_VPG_MODEL_H
#define INFERENCE_VPG_MODEL_H

#include <cstdio>
#include "base_model.h"
#include <torch/script.h>
#include "turtle_shared_memory.h"

class VPGModel: public BaseModel{
private:
    torch::jit::script::Module policy_module;  // PyTorch module
    torch::jit::script::Module value_model;  // PyTorch module

public:
    /*!
     * Load a pytorch model from file and build interpreter.
     * @param policy_model_file_path Policy model torch script for vanilla policy gradient.
     * @param value_model_file_path Value model torch script for vanilla policy gradient.
     * @param turtle_id Turtle ID for turtle communicating with model instance.
     * @return Loading result.
     */
    VPGModel(const char* policy_model_file_path, const char* value_model_file_path, int turtle_id);

    int RunInferenceSampled();
    int RunInferenceGreedy();
};

#endif /* INFERENCE_VPG_MODEL_H */
