#include "shmem.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main()
{   
    shm_message *seg_addr;
    memguard *guard = init_memguard();
    int shm_size = guard->get_shm_size();
    seg_addr = guard->create_segment(SHMEM_KEY, shm_size);
    if (seg_addr == (void*)-1) {
        printf("Cleaning up\n");
        dealloc_memguard(guard);
        return 0;
    }
    //seg_addr[1].ex_flag = no_exit_proc;
    while (getchar() != 'q') {}
    guard->release_segment(SHMEM_KEY, shm_size, seg_addr);
    dealloc_memguard(guard);
    return 0;
}
