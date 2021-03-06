#ifndef SHM_H
    #define SHM_H
    #define BUFF_LEN 100
    #define KEY 12345
    #define DEV_NR 32
    #define KEY 12345
    #define SEM_PATH "/tmp/semaphore"
    #define SEM_PATH2 "/tmp/semaphore2"
enum direction 
{
    outgoing,
    incomming
};
enum message_validity 
{
    valid,
    invalid
};
enum exit_flag
{
    exit_proc,
    no_exit_proc
};
typedef struct shm_message {
    char buff[BUFF_LEN];
    enum message_validity flag;
    int speed;
    int index;
    enum direction packet_direction;
    enum exit_flag ex_flag;
}shm_message;
#endif
