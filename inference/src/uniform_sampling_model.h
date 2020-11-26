/*!
 * uniform_sampling_model.h
 *
 * Perform inference by sampling uniformly from action distribution.
 *
 *  Created on: Oct 4th, 2020
 *      Author: Mickey Beurskens
 */

#ifndef INFERENCE_UNIFORM_SAMPLING_MODEL_H
#define INFERENCE_UNIFORM_SAMPLING_MODEL_H

#include <cstdio>
#include "base_model.h"
#include <torch/script.h>
#include "turtle_shared_memory.h"

class UniformSamplingModel: public BaseModel{
public:
    /*!
     * Sample uniformly from available actions.
     * @param turtle_id Turtle ID for turtle communicating with model instance.
     * @return Loading result.
     */
    UniformSamplingModel(int turtle_id);

    /*!
     * Run model inference and return model output.
     */
    int RunInference();

    int RunInferenceGreedy();
    int RunInferenceSampled();
};

#endif /* INFERENCE_UNIFORM_SAMPLING_MODEL_H */
