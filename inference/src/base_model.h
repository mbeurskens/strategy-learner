/*!
 * base_model.h
 *
 * Base class used to load tensorflow gradient lite models and perform model inference.
 *
 *  Created on: Sep 17, 2020
 *      Author: Mickey Beurskens
 */

#ifndef INFERENCE_BASE_MODEL_H
#define INFERENCE_BASE_MODEL_H

#include <cstdio>
#include <torch/script.h>
#include "turtle_shared_memory.h"

class BaseModel{
private:
    torch::jit::script::Module policy_module;  // PyTorch module
    torch::jit::script::Module value_model;  // PyTorch module
    int t_id;                       // Turtle id for inference model

protected:
    int SampleCategorical(at::Tensor distribution);
    int SampleBest(at::Tensor distribution);

public:
    SharedMemoryInterface mem;      // Shared memory

    /*!
     * Load a pytorch model from file and build interpreter.
     * @param turtle_id Turtle ID for turtle communicating with model instance.
     * @return Loading result.
     */
    BaseModel(int turtle_id);
    virtual ~BaseModel();

    /*!
     * Run model inference and return model output sampled from the predicted distribution.
     */
    virtual int RunInferenceSampled();

    /*!
     * Run model inference and choose output with the highest probability.
     */
    virtual int RunInferenceGreedy();

    /*!
     * Read the observation in shared memory and return it as input vector for the inference model.
     * @return Vector containing input values to the inference model.
     */
    std::vector<torch::jit::IValue> GetModelInputTensor();
};

#endif /* INFERENCE_BASE_MODEL_H */
