#include "iface.h"
#include "sniffer.h"
#include <sys/shm.h>
#include <ifaddrs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

void readInput(char *iface_name)
{
	int character = 0;
	int i = 0;
	while (1) {
		character = getc(stdin);
		if (character == EOF)
            return;
		if (character  == '\n')
            return;
		if (i < IF_NAME_LEN) 
			iface_name[i] = (char)character;
		i += 1;
	}
}

int main(int argc, char **argv) 
{   
    if (argc < 2) {
        printf("Usage: ./<prog name> <interface name>\n");
        printf("example: ./analyzer lkms1\n");
        printf("To obtain list of available interfaces: type 'i' and then press enter\n");
        if (getc(stdin) == 'i') {
            char *if_name = malloc(sizeof(char)*IF_NAME_LEN); /*I'm assuming interface name would be no longer than 25 bytes */
            memset(if_name, 0, IF_NAME_LEN);
	        iflist *if_list = NULL;
   	        if_list = initIflist();
   	        if_list = if_list->getIface(if_list, if_name);
            deallocIflist(if_list);
            free(if_name);
        }
        return -1;
    }
    char *if_name = malloc(sizeof(char)*IF_NAME_LEN); /* I'm assuming interface name would be no longer than 25 bytes */
    memset(if_name, 0, IF_NAME_LEN);
   	iflist *if_list = initIflist();
   	if_list = if_list->getIface(if_list, if_name);
    if (if_list == NULL) {
        printf("Cannot obtain interface list\n");
        return -1;
    }
	int sock_raw = if_list->bindIface(if_name, if_list);
    if (sock_raw == -1) {
        printf("Cannot bind to interface\n");
        return -1;
    }
    sniffer *my_sniffer = initSniffer();
    my_sniffer->sniff(sock_raw, if_name);

    deallocSniffer(my_sniffer);
    free(if_name);
    return 0;
}
