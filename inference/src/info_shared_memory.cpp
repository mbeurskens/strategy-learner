/*!
 * info_shared_memory.cpp
 *
 * Interface to info shared memory in the strategy learner. Communicate turtle shared memory status.
 *
 *  Created on: Jul 16, 2020
 *      Author: Mickey Beurskens
 */


#include "info_shared_memory.h"

extern "C" {
#include "sl_shared_memory.h"
}

InfoShm::InfoShm(int t_id, bool is_client) {
    initialize_info_shm(t_id, (int)is_client, &i_shm);
}

InfoShm::~InfoShm() {
    clear_info_shm(&i_shm);
}

void InfoShm::Write(turtle_info t_info) {
    write_info_shm(t_info, &i_shm);
}

turtle_info InfoShm::Read() {
    return read_info_shm(&i_shm);
}

bool InfoShm::GetTurtleRunningFlag() {
    return (bool)read_info_shm(&i_shm).running_flag;
}