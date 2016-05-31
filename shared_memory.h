#ifndef SHARED_MEMORY_H_
#define SHARED_MEMORY_H_
/*---------------------------------------------------------------------------*/
#include <sys/shm.h>
#include "fcgi_config.h"
#include "fcgiapp.h"
/*---------------------------------------------------------------------------*/
#define MAX_SIZE_ROWS 2000000
#define SHARED_MEMORY 1000
/*---------------------------------------------------------------------------*/
struct shared_use_st
{
    int boost   [MAX_SIZE_ROWS];
    int user_id [MAX_SIZE_ROWS];
} shared_use_st;
/*---------------------------------------------------------------------------*/
struct shared_use_st *SharedMemory()
{
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_data;
    int shmid;

    shmid = shmget((key_t)SHARED_MEMORY, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    shared_data = (struct shared_use_st *)shared_memory;

    return shared_data;
}
/*---------------------------------------------------------------------------*/
#endif /*SHARED_MEMORY_H_*/
