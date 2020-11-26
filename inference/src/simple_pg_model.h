/*!
 * simple_pg_model.h
 *
 * Load tensorflow lite simple policy gradient models and perform model inference.
 *
 *  Created on: Jun 25, 2020
 *      Author: Mickey Beurskens
 */

#ifndef INFERENCE_SIMPLE_PG_MODEL_H
#define INFERENCE_SIMPLE_PG_MODEL_H

#include <cstdio>
#include "base_model.h"
#include <torch/script.h>
#include "turtle_shared_memory.h"

class SimplePGModel: public BaseModel{
private:
    torch::jit::script::Module module;  // PyTorch module
public:
    /*!
     * Load a pytorch model from file and build interpreter.
     * @param filename Path to the torch script file.
     * @param turtle_id Turtle ID for turtle communicating with model instance.
     * @return Loading result.
     */
    SimplePGModel(const char* filename, int turtle_id);

    int RunInferenceGreedy();
    int RunInferenceSampled();
};

#endif /* INFERENCE_SIMPLE_PG_MODEL_H */
