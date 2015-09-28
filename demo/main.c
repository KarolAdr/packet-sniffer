#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

static int spawn_process(char *proc_path, char *proc_name)
{
    pid_t pid, pid_child;
    pid = fork();
    //child
    if (pid == 0) {
        if (execl(proc_path, proc_name," ",(char*)NULL) == -1) { /* TODO hide this ugly path name under nice interface */
            printf("cannot execute%s \n", proc_name);
            return -1;
        }
    }
    else if(pid == -1) {
        /* an error occured during fork */ 
        printf("cannot create new process\n");
        return -1;
    }//parent 
    else {
        //int status = 0;
        pid_child = pid;
        //waitpid(-1, &status, 0);
        return pid_child;
    }
    return 0;
}

int main()
{   
    char analyzer_path[] = "../spawn_analyzer/spawn_analyzer";
    char analyzer_name[] = "spawn_analyzer";

    char semaphore_path[] = "../semaphore/semaphore";
    char semaphore_name[] = "semaphore";

    char shm_path[] = "../shmemory_guard/memory_guard";
    char shm_name[] = "memory_guard";
    /*start all of processes that are necessary for visualizer to work */
    /* (semaphore, shared memory, spawner, visualizer)                 */
    int child_id = spawn_process(semaphore_path, semaphore_name);
    if (child_id == -1) {
        printf("Cannot spawn %s \n", semaphore_name);
        return -1;
    }
    child_id = spawn_process(shm_path, shm_name);
    if (child_id == -1) {
        printf("Cannot spawn %s \n", shm_name);
        return -1;
    }
    child_id = spawn_process(analyzer_path, analyzer_name);
    if (child_id == -1) {
        printf("Cannot spawn %s \n", analyzer_name);
        return -1;
    }
    while (1) {
        pid_t res = wait(0);
        if (res == -1) {
            printf("%s\n", strerror(errno));
            return -1;
        } 
    }
    return 0;
}
