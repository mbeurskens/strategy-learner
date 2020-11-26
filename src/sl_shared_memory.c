/*!
 * sl_shared_memory.c
 *
 * Interface to shared memory functionality.
 *
 *  Created on: Jul 7, 2020
 *      Author: Mickey Beurskens
 */

#include "sl_shared_memory.h"
#include "shared_memory.h"

void initialize_shared_memory(int turtle_id, int client, turtle_shm* t_shm) {
    t_shm->is_client = client;

	if (t_shm->is_client){
        InitializeShm_client(get_obs_shm_id(turtle_id), sizeof(sl_model_input_t),
        		&t_shm->obs_shm, &t_shm->obs_sem_id);
        InitializeShm_client(get_pred_shm_id(turtle_id), sizeof(sl_model_output_t),
        		&t_shm->pred_shm, &t_shm->pred_sem_id);
    } else {
	    InitializeShm_server(get_obs_shm_id(turtle_id), sizeof(sl_model_input_t),
	    		&t_shm->obs_shm, &t_shm->obs_sem_id);
        InitializeShm_server(get_pred_shm_id(turtle_id), sizeof(sl_model_output_t),
        		&t_shm->pred_shm, &t_shm->pred_sem_id);
    }

	t_shm->obs = (sl_model_input_t*)t_shm->obs_shm;
    t_shm->pred = (sl_model_output_t*)t_shm->pred_shm;
}

void write_obs_shared_memory(sl_model_input_t wr_obs, turtle_shm* t_shm) {
    if(SemaphoreWait(t_shm->obs_sem_id) == 0) {
        *t_shm->obs = wr_obs;
        SemaphorePost(t_shm->obs_sem_id);
    }
}

sl_model_input_t read_obs_shared_memory(turtle_shm* t_shm) {
	return *t_shm->obs;
}

void write_pred_shared_memory(sl_model_output_t wr_pred, turtle_shm* t_shm) {
    if(SemaphoreWait(t_shm->pred_sem_id) == 0) {
        *t_shm->pred = wr_pred;
        SemaphorePost(t_shm->pred_sem_id);
    }
}

sl_model_output_t read_pred_shared_memory(turtle_shm* t_shm) {
	return *t_shm->pred;
}

int get_obs_shm_id(int turtle_id)
{
    return turtle_id;
}

int get_pred_shm_id(int turtle_id)
{
    // Return max ID value minus turtle ID
    return 255-turtle_id;
}

void clear_shared_memory(int turtle_id, turtle_shm* t_shm) {
    if (t_shm->is_client) {
        CleanupShm_client(t_shm->obs_shm);
        CleanupShm_client(t_shm->pred_shm);
    } else {
        CleanupShm_server(t_shm->obs_shm, get_obs_shm_id(turtle_id),
        		sizeof(sl_model_input_t));
        CleanupShm_server(t_shm->pred_shm, get_pred_shm_id(turtle_id),
        		sizeof(sl_model_output_t));
    }
}

void write_info_shm(turtle_info t_info, info_shm* shm)
{
    if(SemaphoreWait(shm->info_sem_id) == 0) {
        *shm->t_info = t_info;
        SemaphorePost(shm->info_sem_id);
    }
}

void write_info_shm_blocking(turtle_info t_info, info_shm* shm)
{
    if(SemaphoreWaitBlocking(shm->info_sem_id) == 0) {
        *shm->t_info = t_info;
        SemaphorePost(shm->info_sem_id);
    }
}

turtle_info read_info_shm(info_shm* shm)
{
    return *shm->t_info;
}

void initialize_info_shm(int turtle_id, int client, info_shm* shm)
{
    shm->is_client = client;
    shm->info_shm_id = 100+turtle_id;

    if (shm->is_client){
        InitializeShm_client(shm->info_shm_id, sizeof(turtle_info),
        		(void*)&shm->t_info, &shm->info_sem_id);
    } else {
        InitializeShm_server(shm->info_shm_id, sizeof(turtle_info),
        		(void*)&shm->t_info, &shm->info_sem_id);
    }
}

void clear_info_shm(info_shm* shm)
{
    if (shm->is_client) {
        CleanupShm_client((void*)shm->t_info);
    } else {
        CleanupShm_server((void*)shm->t_info, shm->info_shm_id,
        		sizeof(turtle_info));
    }
}


