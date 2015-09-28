#ifndef SHMEM_H
    #define SHMEM_H
    #include "shm.h"
    #define DEV_NR 32
    #define SHMEM_BUFF_LEN 100
    #define SHMEM_KEY 12345
typedef struct memguard {
    void(*check_mem)(shm_message*);
    void *(*create_segment)(int,int); 
    void (*release_segment)(int,int,shm_message*);
    int (*get_shm_size)(void);
}memguard;

memguard *init_memguard(void);
void dealloc_memguard(memguard*);
#endif
