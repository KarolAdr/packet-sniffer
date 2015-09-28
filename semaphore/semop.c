#include "semop.h"
#include "shm.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

typedef struct semaphore {
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
    int count;
    int flag;
}semaphore;

typedef struct conditional {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int flag;
}conditional;

semaphore *init_mutex()
{   
    semaphore *semap;
    conditional *cond;
    pthread_mutexattr_t attrmutex;
    pthread_condattr_t attrcond;
    int fd = open(SEMAPHORE_1, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (fd < 0) {
        printf("Cannot open/create semaphore file\n");
        printf("%s\n", strerror(errno));
        int remove_res = remove(SEMAPHORE_1);
        if (remove_res != 0) {
            printf("Cannot delete %s\n file", SEMAPHORE_1);
        }
        //return NULL;
    }
    int fd_cond = open(SEMAPHORE_2, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (fd < 0) {
        printf("Cannot open/create conditional file\n");
        printf("%s\n", strerror(errno));
        int remove_res = remove(SEMAPHORE_2);
        if (remove_res != 0) {
            printf("Cannot delete %s\n file", SEMAPHORE_2);
        }
        exit(0);
        //return NULL;
    }
    (void) ftruncate(fd, sizeof(semaphore));
    (void) ftruncate(fd_cond, sizeof(conditional));
    (void) pthread_mutexattr_init(&attrmutex);
    (void) pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);
    (void) pthread_condattr_init(&attrcond);
    (void) pthread_condattr_setpshared(&attrcond, PTHREAD_PROCESS_SHARED);

    semap = (semaphore *)mmap(NULL, sizeof(semaphore), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    cond = (conditional *)mmap(NULL, sizeof(conditional), PROT_READ | PROT_WRITE, MAP_SHARED, fd_cond, 0);
    close(fd);
    close(fd_cond);
    
    (void) pthread_mutex_init(&semap->pmutex, &attrmutex);
    (void) pthread_mutex_init(&cond->mutex, &attrmutex);
    (void) pthread_cond_init(&semap->pcond, &attrcond);
    (void) pthread_cond_init(&cond->cond, &attrcond);
    semap->flag = 0;
    cond->flag = 0;
    return semap;
}

semaphore *mutex_open(char *name)
{
   int fd;
   semaphore *semap;
   fd = open(name, O_RDWR, 0666);
   if (fd < 0) {
        printf("cannot open mutex\n");
        return NULL;
   }
   semap = (semaphore *)mmap(NULL, sizeof(semaphore), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   close(fd);
   return semap;
}

conditional *cond_open(char *name)
{
   int fd;
   conditional *cond;
   fd = open(name, O_RDWR, 0666);
   if (fd < 0) {
        printf("Cannot open mutex2\n");
        return NULL;
   }
   cond = (conditional *)mmap(NULL, sizeof(conditional), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   close(fd);
   return cond;
}

void post(semaphore *semap, conditional *cond, shm_message *message )
{
    pthread_mutex_lock(&semap->pmutex);
    cond->flag = 1;
    if ( cond->flag == 1 )      
        pthread_cond_signal(&cond->cond);
}

void unlock(semaphore *semap)
{
    pthread_mutex_unlock(&semap->pmutex);
}

void wait(conditional *cond, shm_message *message)
{   
    pthread_mutex_lock(&cond->mutex);
        while (cond->flag != 1 )
            pthread_cond_wait(&cond->cond, &cond->mutex);
}

void stop_wait(conditional *cond, shm_message *message)
{
        message[0].flag = invalid;
        cond->flag = 0;
    pthread_mutex_unlock(&cond->mutex);
}

void close_mutex(semaphore *semap)
{
    munmap((void*)semap, sizeof(semaphore));
}
