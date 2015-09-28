#include "visualizer.h"             /* struct visualizer, constructor and destructor */
#include "shm.h"                    /* shm_message struct and others */
#include <sys/ipc.h>                /* inter process    */
#include <sys/shm.h>                /* shared memory    */
#include <sys/mman.h>
#include <stdlib.h>                 /* malloc(...)       */
#include <curses.h>                 /* terminal control  */
#include <pthread.h>                /* thread and synchr */
#include <traffic-visualizer.h>     /* visualizer calls  */
#include <time.h>                   /* clock(...)        */
#include <unistd.h>
#include <string.h>                 /* strerror(..)      */
#include <net/ethernet.h>           /* h_source, h_dest  */
#include <errno.h>                  /* errno value       */

int exit_flag = 0;
char eth_source[] = "[ETH SOURCE ADDRESS]";
char eth_dest[] = "[ETH DESTINATION ADDRESS]";
struct ethhdr *eth;
char mac_addr[64];

static struct traffic_visualizer *create_visualizer() 
{   
    struct traffic_visualizer *vis = create_traffic_visualizer(32, 1, 2);
    int i = 0;
    for (; i < PORTS_NUM; ++i) { 
        vis->activate_port(vis, i, "lkms");
    }
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    initscr(); //very nice library...at the end of program there are "just" 198,848 bytes still accessible(memory leak initscr() )...
    noecho();
    nodelay(stdscr, 1);
    start_color();
    return vis;
}

static void *get_seg_addr()
{  
    /* method for obtaining shared segment address */
    size_t size = sizeof(shm_message)*2; 
    int id = shmget(KEY, size, 0666);
    if (id == -1) {
        printf("Cannot locate shared memory\n");
        return NULL;
    }
    /* attach segment to process so it can have access to it*/
    shm_message *seg_addr = shmat(id, NULL, 0);
    if (seg_addr == (void*)-1) {
        printf("Cannot attache process to shared memory\n");
        return NULL;
    }
    return seg_addr;
}

static void monitor(shm_message *segaddr)
{   
    /* monitoring if user pressed 'q' button          */
    /* if so, set global variable end clean resources */
    char exit_message[] = "To stop press 'q'";
    mvprintw(1, 70, exit_message);
    while (1) {
        if (getchar() == 'q') {
            exit_flag = 1;
            //return;
        }
    }
}

static void print_frame()
{   
    int count = 0;
    for (; count < COLS; ++count) {
        mvprintw(33, count, "-");
    }
}

static void print_frame_in()
{
    mvprintw(37, 3, eth_source);
    mvprintw(37, sizeof(eth_source)+8, eth_dest);
    attron(COLOR_PAIR(3));
    int count = 0;

    for (; count < 2; ++count) {
        mvprintw(36+count, 1, "*");
    }
    count = 0;
    for (; count < sizeof(eth_source)+sizeof(eth_dest)+7; ++count) {
        mvprintw(36, count+1, "*");
    }
    count = 0;
    for (; count < 2; ++count) {
        mvprintw(36+count, sizeof(eth_source) + sizeof(eth_dest)+8, "*");
    }
    attroff(COLOR_PAIR(3));
}

static void print_frame_out()
{   
    int count = 0;
    mvprintw(37, sizeof(eth_source)+sizeof(eth_dest)+13, eth_source);
    mvprintw(37, sizeof(eth_source)*2+sizeof(eth_dest)+19, eth_dest);
    attron(COLOR_PAIR(3));
    for (; count < 2; ++count) {
        mvprintw(36+count, sizeof(eth_source) + sizeof(eth_dest)+11, "*");
    }
    for (; count < sizeof(eth_source)+sizeof(eth_dest)+10; ++count) {
        mvprintw(36, sizeof(eth_source)+sizeof(eth_dest)+count+10, "*");
    }
    count = 0;
    for (; count < 2; ++count) {
        mvprintw(36+count, sizeof(eth_source)*2 +71, "*");
    }
    attroff(COLOR_PAIR(3));
}

static void print_eth_in()
{   
    int count = 0;
    for (; count < sizeof(eth->h_source); ++count) {
            //mvprintw("%02X", eth->h_source[count]
            mac_addr[count] = eth->h_source[count];
            mvprintw(38, (count*3)+5, "%02X", mac_addr[count]);
            mvprintw(38, (count*3)+7, ":");
    }
    count = 0;
    for (; count < sizeof(eth->h_source); ++count) {
        //mvprintw("%02X", eth->h_source[count]
        mac_addr[count] = eth->h_dest[count];
        mvprintw(38, (count*3)+32, "%02X", mac_addr[count]);
        mvprintw(38, (count*3)+34, ":");
    }
}

static void print_eth_out()
{   
    int count = 0;
    for (; count < sizeof(eth->h_source); ++count) {
            //mvprintw("%02X", eth->h_source[count]
            mac_addr[count] = eth->h_source[count];
            mvprintw(38, (count*3)+62, "%02X", mac_addr[count]);
            mvprintw(38, (count*3)+64, ":");
    }
    count = 0;
    for (; count < sizeof(eth->h_source); ++count) {
        //mvprintw("%02X", eth->h_source[count]
        mac_addr[count] = eth->h_dest[count];
        mvprintw(38, (count*3)+90, "%02X", mac_addr[count]);
        mvprintw(38, (count*3)+92, ":");
    }
}

static void visualize(struct traffic_visualizer *vis)
{   
    /* main method in which actual visualizing take place */
    shm_message *segaddr = (shm_message*)get_seg_addr();
    if (segaddr == NULL) {
        printf("Error while accessing shared memory\n");
        endwin();
        return;
    }
    /* monitor is a thread working independently so it would be protected from blocking calls */
    pthread_t exit_monitor; 
    int res = pthread_create(&exit_monitor, NULL, (void*)(monitor), (void*)segaddr);
    if (res) {
        printf("An error occured during thread creation\n");
        endwin();
        return;
    }
    /* opening semaphore, mapped as a file to /tmp/ directory */
    semaphore *semap = mutex_open(SEM_PATH);
    if (!semap) {
        printf("An error occured during semaphore opening\n");
        endwin();
        return;
    }
    /* the same as above    */
    conditional *cond = cond_open(SEM_PATH2);
    if (!semap) {
        printf("An error occured during semaphore2 opening\n");
        endwin();
        return;
    }
    int entry = 0;
    long prev_time = 0;
    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    shm_message tmp;
    //char index[8];
    //char time_char[8];
    //char mac_addr[64];
    memset(mac_addr, 0, sizeof(mac_addr));
    //char out[] = "[OUTGOING PACKET NUMBER]";
    //char in[] = "[INCOMING PACKET NUMBER]";
    //char eth_source[] = "[ETH SOURCE ADDRESS]";
    //char eth_dest[] = "[ETH DESTINATION ADDRESS]";
    //struct ethhdr *eth;
    print_frame();
    while (exit_flag != 1) {
        /* waiting for message from sniffer */
        wait(cond, segaddr);
            eth = (struct ethhdr*)segaddr[0].buff; 
            tmp = segaddr[0]; /* message arrived, process it */
            if (entry == 0) {
                entry = 1;
                prev_time = clock();
            }/*process message */
            while ( ( (clock() - prev_time)/1000) < 100) {
                if (tmp.packet_direction == outgoing) {
                    print_frame_out();
                    print_eth_out();
                    vis->render(vis);
                    vis->set_tx_bitrate(vis, tmp.index, 600);
                    refresh();
                }
                else if (tmp.packet_direction == incomming) {
                    print_frame_in();
                    print_eth_in();
                    vis->render(vis);
                    vis->set_rx_bitrate(vis, tmp.index, 600);
                    refresh();
                }
            }
            if (tmp.packet_direction == incomming)
                vis->set_rx_bitrate(vis, tmp.index, 0);
            else if (tmp.packet_direction == outgoing);
                vis->set_tx_bitrate(vis, tmp.index, 0);
            refresh();
            entry = 0;
        /* release mutex */
        stop_wait(cond, segaddr);
        /* unlock mutex on which sniffer is waiting thanks to which synchronization is provided */
        unlock(semap);
    }
    /*job finished, clear resources*/
    endwin();
    int detach_res = shmdt(segaddr);
    if (detach_res != 0) {
        printf("Detaching process error\n");
        printf("%s ", strerror(errno));
    }
    return;
}

visualizer *init_visualizer()
{   
    visualizer *vis = malloc(sizeof(visualizer));
    vis->visualize = visualize;
    vis->create_visualizer = create_visualizer;
    vis->get_seg_addr = get_seg_addr;
    return vis;
}

void dealloc_visualizer(visualizer* vis)
{
    free(vis);
    vis = NULL;
}
