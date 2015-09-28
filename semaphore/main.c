#include "semop.h"
#include "shm.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

int main()
{   
    semaphore *semap;
    semap = init_mutex();
    while (getchar() != 'q') {}
    //remove /tmp/semaphore files
    int remove_res = remove(SEMAPHORE_1);
    if (remove_res != 0) {
        printf("Cannot remove %s file\n", SEMAPHORE_1);
    }
    remove_res = remove(SEMAPHORE_2);
    if (remove_res != 0) {
        printf("Cannot remove %s file\n", SEMAPHORE_2);
    }
    close_mutex(semap);

    return 0;
}
