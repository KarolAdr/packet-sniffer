#include <stdlib.h>
#include "iface.h"
#include "sniffer.h"
#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

typedef struct ifname {
    char if_name[IF_NAME_LEN];
}ifname;

static iflist* getIface(iflist *iface, char *ifname) 
{
    /* method responsible for obtaining available interfaces on system */
    struct ifaddrs *ifap, *tmp;
    int res;
    res = getifaddrs(&ifap);
    if (res != 0) {
        return NULL;
    }
    tmp = ifap;
    int i = 0;
    while (tmp) {
        if (tmp->ifa_addr->sa_family == AF_PACKET) {
            printf("iface: %s\n", tmp->ifa_name);
            if (i < IFACE_NUM) { 
                if (strlen(tmp->ifa_name) <= sizeof(iface->iface_name->if_name)) { /*check if interface name length is no longer than 25 bytes */
                    memset(iface->iface_name[i].if_name, 0, sizeof(iface->iface_name->if_name));
                    strcpy(iface->iface_name[i].if_name, tmp->ifa_name);
                }
                i += 1;
            }
        }
        tmp = tmp->ifa_next;
    }/* copying name of interface(lkms[some_number]) to char *ifname */
    strcpy(ifname, iface->iface_name[i-1].if_name);

    freeifaddrs(ifap);
    return iface;
}

static int bindIface(char *ifname, iflist *iface)
{   
    /* as name suggests, function responsible for binding socket to interface */
	int i = 0;
    struct ifreq ifr;
    int sock_raw;
	//traverse through ifname struct to find out if iface passed in parameter is available
	if (iface) {
        if (strlen(ifname) > IF_NAME_LEN) {
            return -1;
        }
		while (strcmp(ifname, iface->iface_name[i].if_name) != 0) { 
			i += 1;
			//i've allocated 100 structures for storing interfaces name(it means that if someone got more than this amount of interfaces, we're done..)
			if (i == IFACE_NUM - 1)
                break;
        }
        i = 0;
        strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
        //create socket and bind it to interface 
        sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
        if (sock_raw < 0) {
            printf("Cannot create socket\n");
            return -1;
        }
        //bind to interface 
        if (setsockopt(sock_raw, SOL_SOCKET, SO_BINDTODEVICE, /*(void*)&ifr,(void*)&ifr.ifr_name*/ifname, IFNAMSIZ ) < 0 ) {
            printf("Cannot bind socket to interface! ");
            printf("%s\n", strerror(errno));
            return -1;
        }
        return sock_raw;
    }
    return -1;
}

iflist *initIflist()
{
    iflist *if_list = malloc(sizeof(iflist));
    if_list->iface_name = malloc(sizeof(ifname) * IFACE_NUM); //alloc size for up to 100 ifaces..i think it's enough

    if_list->getIface = getIface; 
    if_list->bindIface = bindIface;
    return if_list;
}

void deallocIflist(iflist *iface)
{   
    free(iface->iface_name);
    free(iface);
}
