#include "sniffer.h"
#include "shm.h"
#include <traffic-visualizer.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/types.h> 
#include <sys/socket.h>  
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <net/if.h>
#include <time.h>
#include <errno.h>

static int getTime()
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return tp.tv_sec;
}

static void *getShmemAddr()
{   
    /* obtaining shared segment address */
    size_t size = sizeof(shm_message) * 2;
    int id = shmget(KEY, size, 0666);
    if (id == -1) {
        printf("cannot locate shared memory\n");
        return NULL;
    }
    /* attach to this process adress space */
    shm_message *seg_addr = shmat(id, NULL,0);
    if (seg_addr == (void*)-1) {
        printf("attaching error\n");
        return NULL;
    }
    return seg_addr;
}

static void sniff(socket_t socketfd, char *ifname)
{   
    /* function for sniffing...                                                                      */
    /* its work in such a way that all packets that are comming in, or out are send to shared memory */
    /* then notify another process takes place(visualizer) to take this packet and process it        */
    struct sockaddr_ll socket_addr;
    long int update_time = 0;
    int out_packnum = 0; 
    int in_packnum = 0;
    int msg_len = 0;
    int total_len = 0;
    struct timespec tp;
    char buff[BUFF_LEN];
    
    socklen_t sockaddr_size = sizeof(socket_addr);
    /* get shmem id and attach process to it and return address to shmem segment */
    shm_message *seg_addr = (shm_message*)getShmemAddr();
    shm_message message;
    semaphore *semap;
    conditional *cond;
    /* opening semaphore for synchronization */
    semap = mutex_open(SEM_PATH);
    if (semap == NULL) {
        printf("Cannot open semaphore\n");
        return;
    }
    /* the same as above */
    cond = cond_open(SEM_PATH2);
    if (semap == NULL) {
        printf("Cannot open semaphore\n");
        return;
    }
    int prev_time = 0;
    int entry_num = 0;
    int packet_length = 0;
    int speed  = 0;
    /* main loop in which sniffing actually takes place */
    while(1) {
        /* time measurement, needed for checking speed at which packet travel across line */
        if (entry_num == 0) {
            entry_num += 1;
            prev_time = getTime();
        }
        /* wait for message(packet) to arrive */
        msg_len = recvfrom(socketfd, buff, BUFF_LEN, 0, (struct sockaddr*)&socket_addr, (socklen_t*)&sockaddr_size);
        /* some message arrived               */
        /* now check how many bytes arrived   */
        /* during one second                  */
        total_len += msg_len;
        if (packet_length == 0) {
            packet_length = total_len;
        } /* data either coming in or out of chosen interface */
        if ((socket_addr.sll_ifindex == if_nametoindex(ifname)) ) {
            if ((int)socket_addr.sll_pkttype == PACKET_OUTGOING) {
                out_packnum += 1;
                clock_gettime(CLOCK_REALTIME, &tp);
                update_time = tp.tv_sec;
                if ( update_time >= prev_time + 1) {
                    speed = total_len - packet_length;
                    packet_length = 0;
                    entry_num = 0;
                }
                message.flag = valid;
                message.speed = speed;
                message.index = socket_addr.sll_ifindex - 4;
                message.packet_direction = outgoing;
                strncpy(message.buff, buff, sizeof(buff));
                post(semap, cond, &message);
                    seg_addr[0] = message;
                /*printf("OUTGOING- lkms[%d",message.index);
                printf("] \n");*/
            }
            else {
                in_packnum += 1;
                clock_gettime(CLOCK_REALTIME, &tp);
                update_time = tp.tv_sec;
                if ( update_time >= prev_time + 1) {
                    speed = total_len - packet_length;
                    packet_length = 0;
                    entry_num = 0;
                }
                message.flag = valid;
                message.speed = speed;
                message.index = socket_addr.sll_ifindex - 4;
                message.packet_direction = incomming;
                strncpy(message.buff, buff, sizeof(buff));
                post(semap, cond, &message);
                    seg_addr[0] = message;
                /*printf("INCOMMING- lkms[%d",message.index);
                printf("] \n");*/
            }
        }
        else if ((socket_addr.sll_ifindex != if_nametoindex(ifname)) ) {
            if ((int)socket_addr.sll_pkttype == PACKET_OUTGOING) {
                printf("outgoing packet another iface\n");
            }
            else {
                printf("incoming from another iface\n");
            }
        }
    }
}

sniffer *initSniffer()
{
    sniffer *my_sniffer = malloc(sizeof(sniffer));
    my_sniffer->sniff = sniff;
    my_sniffer->getShmemAddr = getShmemAddr;
    return my_sniffer;
}

void deallocSniffer(sniffer *sniff)
{
    free(sniff);
}
