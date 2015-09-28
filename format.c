#include "format.h"
#include <stdio.h>
#include <stdlib.h>
#include <net/ethernet.h>

void printInData(char *buff, int packnum, int speed)
{   
    struct ethhdr *eth = (struct ethhdr*)buff;
    //printf("PC PERSPECTIVE [port] <--- --- ---  [ switch ] "); //TODO reverse perspective!
    printf("[INCOMING PACKET NUMBER]");
    printf("      [ETH SOURCE ADDRESS]");
    printf("      [ETH DESTINATION ADDRESS]");
    printf("      [INFO]\n");
    
    printf("\t%d ", packnum);
    int i = 0;
    printf("\t\t\t");
    for (; i < sizeof(eth->h_source); ++i) {
        printf("%02x", eth->h_source[i]);
        printf(":");
    }
    printf("         ");
    i = 0;
    for (; i < sizeof(eth->h_dest); ++i) {
        printf("%02x", eth->h_dest[i]);
        printf(":");
    }
    printf("\t\t%f", (float)speed/1024);
    printf(" kB/s");
    printf("\n\n");
}

void printOutData(char *buff, int packnum, int speed)
{ 
    struct ethhdr *eth = (struct ethhdr*)buff;
    //printf("PC PERSPECTIVE [port] --- --- --- > [ switch ] "); //TODO reverse perspective
    printf("[OUTGOING PACKET NUMBER]");
    printf("      [ETH SOURCE ADDRESS]");
    printf("      [ETH DESTINATION ADDRESS]");
    printf("      [INFO]\n");
    printf("\t%d ", packnum);
    int i = 0;
    printf("\t\t\t");
    for (; i < sizeof(eth->h_source); ++i) {
        printf("%02x", eth->h_source[i]);
        printf(":");
    }
    printf("         ");
    i = 0;
    for (; i < sizeof(eth->h_dest); ++i) {
        printf("%02x", eth->h_dest[i]);
        printf(":");
    }
    printf("\t\t%f", (float)speed/1024);
    printf(" kB/s");
    printf("\n\n");
}

printer *initPrinter()
{
    printer *data_printer = malloc(sizeof(printer));
    data_printer->printInData = printInData;
    data_printer->printOutData = printOutData;

    return data_printer;
}

void deallocPrinter(printer* prnt)
{
    free(prnt);
}
