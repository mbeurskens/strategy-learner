/*!
 * info_shared_memory.h
 *
 * Interface to info shared memory in the strategy learner. Communicate turtle shared memory status.
 *
 *  Created on: Jul 16, 2020
 *      Author: Mickey Beurskens
 */


#ifndef INFERENCE_INFO_SHARED_MEMORY_H
#define INFERENCE_INFO_SHARED_MEMORY_H

extern "C" {
#include "sl_shared_memory.h"
}

class InfoShm {
private:
    int t_id;           // Turtle ID for info server
    info_shm i_shm;     // Info shared memory structure

public:
    /*!
     * Create info shared memory.
     * @param t_id Turtle ID for current info shared memory. Determines shm ID.
     * @param is_client True if shared memory is started as client, false if server.
     */
    InfoShm(int t_id, bool is_client);

    /*!
     * Clean up shared memory.
     */
    ~InfoShm();

    /*!
     * Write a message to info shared memory.
     * @param t_info Data structure written to info server and communicated to turtles.
     */
    void Write(turtle_info t_info);

    /*!
     * Get status of the turtle shared memory server.
     * @return Turtle info struct stored in shared memory.
     */
    turtle_info Read();

    /*!
     * Return turtle running flag from shared memory server.
     * @return Turtle running flag.
     */
    bool GetTurtleRunningFlag();
};

#endif //INFERENCE_INFO_SHARED_MEMORY_H
