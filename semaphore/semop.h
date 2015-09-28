#ifndef SEMOPH_H
    #define SEMOP_H
    #define BUFF_LEN 100
    #define SEMAPHORE_1 "/tmp/semaphore"
    #define SEMAPHORE_2 "/tmp/semaphore2"
typedef struct semaphore semaphore;
typedef struct conditional conditional;
typedef struct shm_message shm_message;

semaphore *init_mutex();
semaphore *mutex_open(char*);
conditional *cond_open(char*);
void post(semaphore*,conditional*,shm_message*);
void unlock(semaphore*);
void wait(conditional*,shm_message*);
void stop_wait(conditional*,shm_message*);
void close_mutex(semaphore*);
#endif

