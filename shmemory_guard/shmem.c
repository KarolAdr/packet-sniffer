#include "shmem.h"
#include <string.h>         /* strerror(..)     */
#include <stdio.h>
#include <stdlib.h>         /* malloc(..)       */
#include <unistd.h>         /* sleep(..)        */
#include <sys/ipc.h>        /* shared memory    */
#include <sys/shm.h>        /*                  */
#include <errno.h>

static int get_shm_size()
{
    int shm_size = sizeof(shm_message)*2;
    return shm_size;
}

static void release_segment(int key, int size, shm_message *segaddr)
{   
    int seg_id = shmget(key, size, 0666);
    if (seg_id == -1) {
        printf("Obtaining segment id failed\n");
        printf("%s\n", strerror(errno));
        return;
    }
    /* first-  detach segment     */
    /* and eventually- remove segment   */
    if (shmdt(segaddr) == -1) {
        printf("Cannot detach memory segment\n");
        return;
    }
    if (shmctl(seg_id, IPC_RMID, NULL) == -1) {
        printf("Cannot remove memory segment ");
        printf("%s\n", strerror(errno));
        return;
    }
    printf(" Shared memory segment have been successfully released\n");
}

static void free_segment(int key, int size) 
{   
    int seg_id = shmget(key, size, 0666);
    if (seg_id == -1) {
        printf("Obtaining segment id failed\n");
        printf("%s\n", strerror(errno));
        return;
    }
    if (shmctl(seg_id, IPC_RMID, NULL) == -1) {
        printf("Cannot remove memory segment ");
        printf("%s\n", strerror(errno));
        return;
    }
    printf(" Shared memory segment have been successfully released\n");
}

static void *create_segment(int key, int size)
{
    /* ensure that shared memory will be created                            */
    /* to do so, specify IPC_EXCL + IPC_CREAT                               */
    /* if segment cannot be created(segment already exist), call to shmget  */
    /* will return -1                                                       */
    /* segment can be read or written to by user,group, others              */
    int flags = (IPC_CREAT | IPC_EXCL | 0666);

    int id = shmget(key, size, flags);
    if (id == -1) {
        printf("Segment already exist\n");
        free_segment(key, size);
        return (void*)-1; 
    }
    /* attach segment to calling process                */
    /* and tell system to automatically                 */
    /* align proper address space(second arg = NULL)    */
    shm_message *segaddr = shmat(id, NULL, 0);
    if ((segaddr) == ((void*)-1)) {
        printf("Cannot attach to shared memory\n");
        return (void*)-1;
    }
    /*put something into shared segment to check if everything works properly */
    segaddr->flag = 12345;
    return segaddr;
}

memguard *init_memguard()
{
    memguard *guard = malloc(sizeof(memguard));
    guard->get_shm_size = get_shm_size;
    guard->create_segment = create_segment;
    guard->release_segment = release_segment;
    return guard;
}

void dealloc_memguard(memguard *guard)
{
    free(guard);
}
