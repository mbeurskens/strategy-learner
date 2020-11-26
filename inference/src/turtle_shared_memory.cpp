/*!
 * turtle_shared_memory.cpp
 *
 * Interface to turtle shared memory in the strategy learner.
 *
 *  Created on: Jul 06, 2020
 *      Author: Mickey Beurskens
 */

#include <iostream>
#include "turtle_shared_memory.h"

extern "C" {
#include "sl_data_types.h"
#include "sl_shared_memory.h"
};

SharedMemoryInterface::SharedMemoryInterface(int turtle_id, bool client=true) {
    t_id = turtle_id;
    initialize_shared_memory(turtle_id, (int)client, &t_shm);
}

void SharedMemoryInterface::WriteObs(sl_model_input_t wr_obs) {
    write_obs_shared_memory(wr_obs, &t_shm);
}

sl_model_input_t SharedMemoryInterface::ReadObs() {
    return read_obs_shared_memory(&t_shm);
}

void SharedMemoryInterface::WritePrediction(sl_model_output_t wr_pred) {
    write_pred_shared_memory(wr_pred, &t_shm);
}

sl_model_output_t SharedMemoryInterface::ReadPrediction() {
    return read_pred_shared_memory(&t_shm);
}

SharedMemoryInterface::~SharedMemoryInterface() {
    clear_shared_memory(t_id, &t_shm);
}
