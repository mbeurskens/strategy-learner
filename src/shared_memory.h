#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H
/*
 * shared_memory.h
 *
 *  Created on: Apr 21, 2020
 *      Author: mb
 *
 * This API is intended to make it as easy as possible to use the shared
 * memory inter-process mechanism, which enables highly efficient communication
 * between two (or more) programs.
 *
 * Usage:
 * One of the programs has to act as a server and allocate the memory for
 * the shared memory segment, as well as the semaphore that is used to
 * control the acces to the shared memory segment.
 * In the initialization part of the server program (typically mdlStart),
 * the server has to call 'InitializeShm_server(int shmId, size_t segSize, void** pShmPointer, int* pSemId).
 * The input arguments are:
 * - shmId: an integer between 0 and 255 uniquely identifying the shared
 * 			memory segment. Note: Make sure that the integer of your choice
 * 			is not already in use somewhere else in the progam.
 * - segSize: the required size of the shared memory segment, e.g. sizeof(sfgd).
 * The output arguments are:
 * - pShmPointer: the address of the pointer to the shared memory segment,
 * 				 recommended usage is &(void* shmPointer).
 * - pSemId: the addres to the semaphore identifier, recommended usage is
 * 			 &(int semId).
 *
 * The server can now write to the shared memory segment by writing to shmPointer.
 * Access to the segment should be protected by the semaphore, e.g.
 * if(SemaphoreWait(semId) == 0) {
 *     *shmPointer = 1.234;
 * 	   SemaphorePost(semId);
 * }
 * Two different SemaphoreWait functions are provided one blocking (SemaphoreWaitBlocking)
 * and one non-blocking (SemaphoreWait). The last one is intended for use in
 * real-time programs.
 *
 * The client program(s) can obtain the pointer to shared memory segment
 * and the semaphore ID by calling
 * 'InitializeShm_client(int shmId, size_t segSize, void** pShmPointer, int* pSemId).
 * The input and output arguments are the same as for the server variant
 * of the function.
 * Access to the shared memory segment works the same way as for the server.
 *
 * In the clean up part of the programs (typically mdlTerminate), the shared
 * memory resources have to be freed using CleanupShm_client and CleanupShm_server.
 * If these functions are not called the shared memory segment and semaphore
 * will remain in memory, even after the program has been closed.
 *
 * Troubleshooting:
 * If one of the programs crashed before its resources could be freed, you
 * can use the 'ipcs -m' shell command to display the currently allocated
 * shared memory segments, and 'ipcs -s' to display the semaphores.
 * With 'ipcrm shm <shmKey>', shared memory segments can be removed.
 * Semaphores can be removed using 'ipcrm sem <semKey>'.
 *
 * Written by M. Briegel, Eindhoven 22 July 2015.
 */

/* System includes */
#include <stddef.h>

/* Macros */
#define RET_OK      0
#define SHM_RET_ERROR   1

/* Type definitions */
#ifndef UNION_SENUM_DEFINED
#define UNION_SENUM_DEFINED
union semun
{
	int val;
	struct semid_ds* buf;
	unsigned short int* array;
	struct seminfo* __buf;
};
#endif

/* Interface function prototypes */
int  InitializeShm_client(int shmId, size_t segSize, void** pShmPointer, int* pSemId);
int  InitializeShm_server(int shmId, size_t segSize, void** pShmPointer, int* pSemId);
int  SemaphorePost(int semId);
int  SemaphoreWait(int semId);
int  SemaphoreWaitBlocking(int semId);
void CleanupShm_client(void* shmPointer);
void CleanupShm_server(void* shmPointer, int shmId, size_t shmSize);

#endif
