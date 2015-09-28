#ifndef SNIFFER_H
    #define SNIFFER_H
    #include "semaphore/semop.h"
typedef int socket_t;
typedef struct shm_message shm_message;
typedef struct sniffer {
    void(*sniff)(socket_t, char* );
    struct ifreq(*getIfaddr)(char*);
    void*(*getShmemAddr)(void);
}sniffer;

sniffer *initSniffer(void);
void deallocSniffer(sniffer*);
#endif
