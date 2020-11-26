/*
 * shared_memory.c
 *
 *  Created on: Apr 21, 2020
 *      Author: mb
 */

/* Interface include */
#include "shared_memory.h"

/* System includes */
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>

/* Module includes */

/* Macros */
#define IPC_API_DEBUGMODE
#define SHM_PATH "/home/robocup/"

/* Type definitions */

/* Internal variables */

/* Internal function prototypes */
static int AllocateShmSegment(char* path, int shmId, size_t segSize);
static void* AttachToShmSegment(int segId);
static int CreateSemaphore(char* path, int semNumber);
static int DeallocateShmSegment(int segId);
static int DetachFromShmSegment(void* shmPointer);
static int DisposeSemaphore(int semId);
static int GetSemaphoreId(char* path, int semNumber);
static int GetShmSegmentId(char* path, int shmId, size_t segSize);
static int InitializeSemaphore(int semId, int value);

/* ****************************** *
 * Interface function definitions *
 * ****************************** */
int InitializeShm_client(int shmId, size_t segSize, void** pShmPointer, int* pSemId)
{
    int ret = RET_OK, segId = -1;

    /* check value of semNumber (only the first 8 bits are used -> 0<=shmId<=255) */
    if(shmId<0 || shmId>255)
    {
        printf("ERROR in %s, line %d: the first argument of %s() has to be an int between 0 and 255!\n", __FILE__, __LINE__, __FUNCTION__);
        return SHM_RET_ERROR;
    }

    /* Get the pointer to the shared memory segment */
    if (ret == RET_OK)
    {
        if((segId = GetShmSegmentId(SHM_PATH, shmId, segSize)) < 0)
        {
            printf("ERROR while getting shared memory segment ID!\n");
            ret = SHM_RET_ERROR;
        }
    }
    /* Attach to shared memory segment */
    if (ret == RET_OK)
    {
        if((*pShmPointer = AttachToShmSegment(segId)) == (void*) -1)
        {
            printf("ERROR while attaching to shared memory segment!\n");
            ret = SHM_RET_ERROR;
        }
    }

    /* Get the semaphore identifier */
    if (ret == RET_OK)
    {
        if((*pSemId = GetSemaphoreId(SHM_PATH, shmId)) < 0)
        {
            ret = SHM_RET_ERROR;
        }
    }

    return ret;
}

int InitializeShm_server(int shmId, size_t segSize, void** pShmPointer, int* pSemId)
{
    int ret = RET_OK, segId = -1;

    /* check value of semNumber (only the first 8 bits are used -> 0<=shmId<=255) */
    if(shmId<0 || shmId>255)
    {
        printf("ERROR in %s, line %d: the first argument of %s() has to be an int between 0 and 255!\n", __FILE__, __LINE__, __FUNCTION__);
        ret = SHM_RET_ERROR;
    }
    /* Allocate memory and attach to the shared memory segment */
    if (ret == RET_OK)
    {
        if((segId = AllocateShmSegment(SHM_PATH, shmId, segSize)) < 0)
        {
            printf("ERROR while allocating shared memory segment!\n");
            ret = SHM_RET_ERROR;
        }
    }
    /* Attach to segment */
    if (ret == RET_OK)
    {
        if((*pShmPointer = AttachToShmSegment(segId)) == (void*) -1)
        {
            printf("ERROR while attaching to shared memory segment!\n");
            ret = SHM_RET_ERROR;
        }
    }
    /* Create a system V semaphore and initialize it to 1 */
    if (ret == RET_OK)
    {
        if((*pSemId = CreateSemaphore(SHM_PATH, shmId)) >= 0)
        {
            InitializeSemaphore(*pSemId, 1);
        }
        else
        {
            ret = SHM_RET_ERROR;
        }
    }

    return ret;
}

int SemaphorePost(int semId)
{
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    /* Increment by 1 */
    operations[0].sem_op = 1;
    /* Permit undoing and non-blocking function call */
    operations[0].sem_flg = SEM_UNDO | IPC_NOWAIT;

    return semop(semId, operations, 1);
}

int SemaphoreWait(int semId)
{
    struct sembuf operations[1];
    /* Select the semaphore */
    operations[0].sem_num = 0;
    /* Decrement the semaphore by 1 */
    operations[0].sem_op = -1;
    /* Permit undoing and donot wait */
    operations[0].sem_flg = SEM_UNDO | IPC_NOWAIT;

    return semop(semId, operations, 1);
}

int SemaphoreWaitBlocking(int semId)
{
    struct sembuf operations[1];
    /* Select the semaphore */
    operations[0].sem_num = 0;
    /* Decrement the semaphore by 1 */
    operations[0].sem_op = -1;
    /* Permit undoing and donot wait */
    operations[0].sem_flg = SEM_UNDO;

    return semop(semId, operations, 1);
}

void CleanupShm_client(void* shmPointer)
{
    if(shmPointer != NULL){
        DetachFromShmSegment(shmPointer);
    }
}

void CleanupShm_server(void* shmPointer, int shmId, size_t shmSize)
{
    int segId = -1;
    int semId = -1;

    /* Detach from shared memory segment */
    if(shmPointer != NULL){
        DetachFromShmSegment(shmPointer);
    }
    /* Deallocate shared memory segment */
    segId = GetShmSegmentId(SHM_PATH, shmId, shmSize);
    DeallocateShmSegment(segId);
    /* Dispose of the semaphore */
    semId = GetSemaphoreId(SHM_PATH, shmId);
    DisposeSemaphore(semId);
}


/* ***************************** *
 * Internal function definitions *
 * ***************************** */
static int AllocateShmSegment(char* path, int shmId, size_t segSize)
{
    int segmentId = -1;
    key_t shmKey = (key_t) -1;

    /* check value of shmID (only the first 8 bits are used -> 0<=shmId<=255) */
    if(shmId<0 || shmId>255)
    {
        #ifdef IPC_API_DEBUGMODE
        printf("ERROR in %s, line %d: the second argument of %s() has to be an int between 0 and 255!\n", __FILE__, __LINE__, __FUNCTION__);
        #endif
        return segmentId;
    }

    /* Get unique shared memory key */
    if((shmKey = ftok(path, shmId)) == (key_t) -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("ftok shm");
        #endif
    }
    else
    {
        #ifdef IPC_API_DEBUGMODE
        printf("Succesfully obtained shmKey (=%d)!\n", shmKey);
        #endif
        /* Allocate a shared memory segment */
        if((segmentId = shmget(shmKey, segSize, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)
        {
            #ifdef IPC_API_DEBUGMODE
            perror("shmget");
            #endif
        }
        #ifdef IPC_API_DEBUGMODE
        else
        {
            printf("Succesfully obtained shm segment id (=%d)!\n", segmentId);
        }
        #endif
    }

    return segmentId;
}

static void* AttachToShmSegment(int segId)
{
    void* pSeg = (void*) -1;

    if((pSeg = shmat(segId, NULL, SHM_RND)) == (void*) -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("AttachToShmSegment");
        #endif
    }
    #ifdef IPC_API_DEBUGMODE
    else
    {
        printf("Successfully attached to shared memory segment %d!\n", segId);
    }
    #endif

    return pSeg;
}

static int CreateSemaphore(char* path, int semNumber)
{
    int semId = -1;
    key_t semKey = (key_t) -1;

    /* check value of semNumber (only the first 8 bits are used -> 0<=shmId<=255) */
    if(semNumber<0 || semNumber>255)
    {
        #ifdef IPC_API_DEBUGMODE
        printf("ERROR in %s, line %d: the second argument of %s() has to be an int between 0 and 255!\n", __FILE__, __LINE__, __FUNCTION__);
        #endif
        return semId;
    }

    /* Get semaphore key */
    if((semKey = ftok(path, semNumber)) == (key_t) -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("ftok sem");
        #endif
    }
    else
    {
        #ifdef IPC_API_DEBUGMODE
        printf("semaphore key id: %d\n", semKey);
        #endif
        /* Create semaphore */
        if((semId = semget(semKey, 1, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1)
        {
            #ifdef IPC_API_DEBUGMODE
            perror("semget");
            #endif
        }
        #ifdef IPC_API_DEBUGMODE
        else
        {
            printf("Succesfully created semaphore (%d)\n", semId);
        }
        #endif
    }

    return semId;
}

static int DeallocateShmSegment(int segId)
{
    int returnValue = -1;

    if((returnValue = shmctl(segId, IPC_RMID, 0)) == -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("DeallocateShmSegment");
        #endif
    }
    #ifdef IPC_API_DEBUGMODE
    else
    {
        printf("Succesfully marked shmSegment %d for removal.\n", segId);
    }
    #endif

    return returnValue;
}

static int DetachFromShmSegment(void* shmPointer)
{
    int returnValue = -1;

    if((returnValue = shmdt(shmPointer)) == -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("shmdt");
        #endif
    }
    #ifdef IPC_API_DEBUGMODE
    else
    {
        printf("Successfully detached from shm segment!\n");
    }
    #endif

    return returnValue;
}

static int DisposeSemaphore(int semId)
{
    int returnValue = -1;
    union semun dummySemUnion;

    if((returnValue = semctl(semId, 1, IPC_RMID, dummySemUnion)) == -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("dispose sem");
        #endif
    }
    #ifdef IPC_API_DEBUGMODE
    else
    {
        printf("Successfully removed semaphore %d!\n", semId);
    }
    #endif

    return returnValue;
}

static int GetSemaphoreId(char* path, int semNumber)
{
    int semId = -1;
    key_t semKey = -1;

    /* check value of semNumber (only the first 8 bits are used -> 0<=shmId<=255) */
    if(semNumber<0 || semNumber>255)
    {
        #ifdef IPC_API_DEBUGMODE
        printf("ERROR in %s, line %d: the second argument of %s() has to be an int between 0 and 255!\n", __FILE__, __LINE__, __FUNCTION__);
        #endif
        return semId;
    }

    /* Get a unique semaphore key */
    if((semKey = ftok(path, semNumber)) == (key_t) -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("ftok sem");
        #endif
    }
    else
    {
        #ifdef IPC_API_DEBUGMODE
        printf("Succesfully obtained semKey (=%d)!\n", semKey);
        #endif
        /* Obtain semaphore identifier */
        if((semId = semget(semKey, 1, 0)) == -1)
        {
            #ifdef IPC_API_DEBUGMODE
            perror("semget");
            #endif
        }
        #ifdef IPC_API_DEBUGMODE
        else
        {
            printf("Succesfully obtained semaphore id (=%d)!\n", semId);
        }
        #endif
    }

    return semId;
}

static int GetShmSegmentId(char* path, int shmId, size_t segSize)
{
    int segmentId = -1;
    key_t shmKey = (key_t) -1;

    /* check value of shmID (only the first 8 bits are used -> 0<=shmId<=255) */
    if(shmId<0 || shmId>255)
    {
        #ifdef IPC_API_DEBUGMODE
        printf("ERROR in %s, line %d: the second argument of %s() has to be an int between 0 and 255!\n", __FILE__, __LINE__, __FUNCTION__);
        #endif
        return segmentId;
    }

    /* Get unique shared memory key */
    if((shmKey = ftok(path, shmId)) == (key_t) -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("ftok shm");
        #endif
    }
    else
    {
        #ifdef IPC_API_DEBUGMODE
        printf("Succesfully obtained shmKey (=%d)!\n", shmKey);
        #endif
        /* Grab a shared memory segment */
        if((segmentId = shmget(shmKey, segSize, 0)) == -1)
        {
            #ifdef IPC_API_DEBUGMODE
            perror("shmget");
            #endif
        }
        #ifdef IPC_API_DEBUGMODE
        else
        {
            printf("Succesfully obtained shm segment id (=%d)!\n", segmentId);
        }
        #endif
    }

    return segmentId;
}

static int InitializeSemaphore(int semId, int value)
{
    int returnValue = -1;
    union semun argument;
    unsigned short values[1];

    values[0] = value;
    argument.array = values;

    if((returnValue = semctl(semId, 0, SETALL, argument)) == -1)
    {
        #ifdef IPC_API_DEBUGMODE
        perror("InitializeSemaphore");
        #endif
    }
    #ifdef IPC_API_DEBUGMODE
    else
    {
        printf("Successfully initialized semaphore %d to value %d\n", semId, value);
    }
    #endif

    return returnValue;
}
