/*!
 * sl_shared_memory.h
 *
 *  Created on: Jul 7, 2020
 *      Author: Mickey Beurskens
 */

#ifndef TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SRC_SL_SHARED_MEMORY_H_
#define TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SRC_SL_SHARED_MEMORY_H_

#include "sl_data_types.h"
#include <string.h>

/*!
 * Structure containing parameters for strategy learner shared memory of a turtle.
 */
typedef struct {
    int obs_sem_id;			// Observation semaphore ID
    int pred_sem_id; 		// Role semaphore ID
    void* obs_shm; 			// Void pointer to observation shared memory
    void* pred_shm; 		// Void pointer to role shared memory
    sl_model_input_t* obs; 	// Pointer to observation shared memory
    sl_model_output_t* pred; 	// Pointer to network predictions
    int is_client;          // True when shared memory accessed as client, false if accessed as server
} turtle_shm;

/*!
 * Data communicated over info shared memory.
 */
typedef struct {
	int running_flag;
	int manual_mode_flag;
	char turtle_data_save_dir[512];
} turtle_info;

/*!
 * Communicate turtle shm status over shared memory.
 */
typedef struct {
    int info_sem_id; // Kill switch semaphore ID
    int info_shm_id; // Kill switch shared memory ID
    turtle_info* t_info;
    int is_client;   // True when shared memory accessed as client, false if accessed as server
} info_shm;

/*!
 * Initialize shared memory to interface with inference.
 * @param turtle_id Turtle ID for shm ID.
 * @param client True if shm in client mode, false if in server mode. Server manages shared memory.
 * @param t_shm Turtle shared memory struct. Each turtle needs a separate shared memory struct.
 */
void initialize_shared_memory(int turtle_id, int client, turtle_shm* t_shm);

/*!
 * Write model input as observation to shared memory.
 * @param wr_obs Observation to write to shared memory.
 * @param t_shm Turtle shared memory struct. Each turtle needs a separate shared memory struct.
 */
void write_obs_shared_memory(sl_model_input_t wr_obs, turtle_shm* t_shm);

/*!
 * Read observation shared memory.
 * @param t_shm Turtle shared memory struct. Each turtle needs a separate shared memory struct.
 * @return Observation.
 */
sl_model_input_t read_obs_shared_memory(turtle_shm* t_shm);

/*!
 * Write predictions to shared memory.
 * @param wr_pred Prediction to write to shared memory.
 * @param t_shm Turtle shared memory struct. Each turtle needs a separate shared memory struct.
 */
void write_pred_shared_memory(sl_model_output_t wr_pred, turtle_shm* t_shm);

/*!
 * Read prediction shared memory and return it.
 * @param t_shm Turtle shared memory struct. Each turtle needs a separate shared memory struct.
 * @return Prediction struct.
 */
sl_model_output_t read_pred_shared_memory(turtle_shm* t_shm);

/*!
 * Get the observation shared memory id based on turtle id.
 * @param turtle_id Turtle ID.
 * @return Observation shared memory ID.
 */
int get_obs_shm_id(int turtle_id);

/*!
 * Get the prediction shared memory id based on turtle id.
 * @param turtle_id Turtle ID.
 * @return Prediction shared memory ID.
 */
int get_pred_shm_id(int turtle_id);

/*!
 * Clean shared memory.
 * @param turtle_id Turtle ID for shm ID.
 * @param t_shm Turtle shared memory struct. Each turtle needs a separate shared memory struct.
 */
void clear_shared_memory(int turtle_id, turtle_shm* t_shm);

/*!
 * Write to info shared memory.
 * @param t_shm_running_flag 1 if turtle is running, 0 if shutting down or shut down.
 * @param shm Info shared memory struct.
 */
void write_info_shm(turtle_info t_info, info_shm* shm);

/*!
 * Write to info shared memory in a blocking way.
 * @param t_shm_running_flag 1 if turtle is running, 0 if shutting down or shut down.
 * @param shm Info shared memory struct.
 */
void write_info_shm_blocking(turtle_info t_info, info_shm* shm);

/*!
 * Read Turtle info server.
 * @param shm Info shared memory struct.
 * @return Turtle inference info.
 */
turtle_info read_info_shm(info_shm* shm);

/*!
 * Initialize info shared memory.
 * @param turtle_id Turtle ID for shm ID.
 * @param client True if shm in client mode, false if in server mode. Server manages shared memory.
 * @param shm Info shared memory struct.
 */
void initialize_info_shm(int turtle_id, int client, info_shm* shm);

/*!
 * Clear info shared memory.
 * @param shm Info shared memory struct.
 */
void clear_info_shm(info_shm* shm);

#endif /* TURTLE2_STRATEGY_SRC_STP_STRATEGY_LEARNER_SRC_SL_SHARED_MEMORY_H_ */
