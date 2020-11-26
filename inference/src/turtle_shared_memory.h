/*!
 * turtle_shared_memory.cpp
 *
 * Interface to turtle shared memory in the strategy learner.
 *
 *  Created on: Jul 06, 2020
 *      Author: Mickey Beurskens
 */


#ifndef INFERENCE_TURTLE_SHARED_MEMORY_H
#define INFERENCE_TURTLE_SHARED_MEMORY_H

// Include shared memory functionality
extern "C" {
#include "sl_data_types.h"
#include "sl_shared_memory.h"
};

class SharedMemoryInterface {
private:
    int t_id;           // Turtle ID
    bool is_client;     // Track if shared memory interface functions as a client or server
    turtle_shm t_shm;   // Turtle shared memory parameters

public:
    /*!
     * Initialize shared memory.
     * @param turtle_id Turtle ID. Determines shared memory IDs.
     * @param client Flag to determine whether to open shared memory as client.
     */
    SharedMemoryInterface(int turtle_id, bool client);  // Constructor

    /*!
     * Write an observation to shared memory.
     * @param wr_obs Observation to be written.
     */
    void WriteObs(sl_model_input_t wr_obs);

    /*!
     * Read an observation from shared memory.
     * @return Observation.
     */
    sl_model_input_t ReadObs();

    /*!
     * Write a prediction to shared memory.
     * @param wr_pred Prediction structure role to be written.
     */
    void WritePrediction(sl_model_output_t wr_pred);

    /*!
     * Read a prediction from shared memory.
     * @return Prediction.
     */
    sl_model_output_t ReadPrediction();

    /*!
     * Clean up shared memory.
     * @param turtle_id Turtle ID. Determines shared memory IDs.
     */
    ~SharedMemoryInterface();              // Destructor
};

#endif //INFERENCE_TURTLE_SHARED_MEMORY_H
