/*hmmm...O_PATH linux specific..               */
/*following macro is used to obtain definition */
#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif
#include "spawner.h"
#include <stdio.h>
#include <fcntl.h>     /*                           */
#include <dirent.h>    /*  opendir(..) syscall      */
#include <stdlib.h>    /*  malloc(..) syscall       */
#include <sched.h>     /*  setns(..)                */
#include <unistd.h>    /*  getpid(..) syscall+fork  */
#include <sys/types.h> /*  open(..) syscall         */
#include <sys/wait.h>  /*  wait(..) syscall         */
#include <sys/stat.h>  /*                           */
#include <string.h>    /* strerror, strncat etc     */
#include <errno.h>

static int spawn_analyzer()
{
    /* fork new process and exec analyzer in namespace */
    pid_t pid, pid_child;
    pid = fork();
    //child
    if (pid == 0) {
        if (execl(PATH_ANALYZER, ANALYZER," ",(char*)NULL) == -1) { 
            printf("Cannot execute analyzer\n");
            printf("%s \n", strerror(errno));
            return -1;
        }
    }
    else if(pid == -1) {
        /* an error occured during fork */ 
        printf("Cannot create new process\n");
        return -1;
    }//parent 
    else {
        pid_child = pid;
        return pid_child;
    }
    return 0;
}

static void get_netns()
{
    DIR *dir;
    struct dirent *ent;
    char ns_path[] = PATH_TO_NAMESPACES;  
    int netns_fd = 0;
    int clean_flag = 0;

    char *path_dst = malloc(sizeof(char)*PATH_NAME_SIZE);
    memset(path_dst, 0, PATH_NAME_SIZE);
    strncpy(path_dst, ns_path, sizeof(ns_path)); 

    dir = opendir(ns_path);
    if (!dir) {
        printf("Cannot open directory\n");
        printf("%s\n", strerror(errno));
        return;
    }
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] != '.') {
            path_dst = strncat(path_dst, ent->d_name, NAMESPACE_NAME_SIZE); /* ensure not to copy more bytes than path_dst is able to store(if name of ns will be greater than 16 signs, opening namespace fd would fail) */
        }
        /*printf("%s \n", ent->d_name);
        printf("%s \n", ns_path);
        printf("%s \n", path_dst);*/
        if (strcmp(ns_path, path_dst) != 0) {
            netns_fd = open(path_dst, O_RDONLY);
            if (netns_fd == -1) {
                printf("Opening namespace file error\n");
                clean_flag = 1;
                break;
                //return;
            }
            if (setns(netns_fd, CLONE_NEWNET) == -1) {
                printf("Joining namespace error\n");
                clean_flag = 1;
                break;
                //return;
            }
            /* call spawner to fork new process and then replace it with my analyzer progs image */
            int child_id = spawn_analyzer();
            if (child_id == -1) {
                printf("Process creation failed\n");
                clean_flag = 1;
                break;
                //return;
            }
        }
        if (clean_flag != 1) 
            /* overwrite path_dst to point again to /var/run/netns/ directory */
            strncpy(path_dst, ns_path, sizeof(ns_path));
        
    }
    free(path_dst);
    int dir_close_res = closedir(dir);
    if (dir_close_res == -1) {
        printf("%s\n", strerror(errno));
    }
    /* wait for all child process to terminate */
    while (1) {
        pid_t res = wait(0);
        if (res == -1) {
            printf("%s\n", strerror(errno));
            return;
        }
    }
}

spawner *init_spawner()
{
    spawner *analyzer_spawner = malloc(sizeof(spawner));
    analyzer_spawner->spawn_analyzer = spawn_analyzer;
    analyzer_spawner->get_netns = get_netns;
    return analyzer_spawner;
}

void dealloc_spawner(spawner *spawner)
{
    free(spawner);
}


