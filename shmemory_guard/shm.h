#ifndef SHM_H
    #define SHm_H
    #define BUFF_LEN 100
    #define KEY 12345
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
