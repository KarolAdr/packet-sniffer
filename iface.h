#ifndef IFACE_H
    #define IFACE_H
    #define IFACE_NUM 100
    #define IF_NAME_LEN 25
typedef struct ifname ifname;
typedef struct iflist iflist;
//struct for listing available interfaces
typedef struct iflist {
    iflist* (*getIface)(iflist*,char*);
    int(*bindIface)(char*,iflist*);
    ifname *iface_name; 
}iflist;
//constructor and destructor
iflist* initIflist(void);
void deallocIflist(iflist*);
#endif
