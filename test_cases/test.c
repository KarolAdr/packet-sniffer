#include "test.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>

typedef struct menu_item {
    char item_name[20];
    void(*item_operation)(void);
}menu_item;

typedef struct menu_ports {
    char port_name[20];
    void(*port_operation)(void);
}menu_ports;

int *menu_size;
int *ports_num;
menu_item *m_item;
menu_ports *port;

static menu_item *init_menu(int *menu_size);
static void print_frame(void);
static void add_menu_label(void);
static void add_designer_label(void);
static void print_menu(void);
static void print_menu_items(void);
static void print_port_items(void);
static void monit_keys(void);
static void main_loop(void);

static void main_loop()
{   
    int ch;
    int key_id = 0;
    int j = 0;
    while ( (ch = getch()) != 'q') {
        switch(ch) {
        case KEY_UP:
                if (key_id <= 0)
                    key_id = *menu_size;
                else 
                    key_id -= 1;
            break;
        case KEY_DOWN:
                if (key_id >= *menu_size)
                    key_id = 0;
                else
                    key_id += 1;
            break;
        }
        /*traverse through menu_item structure to print colored menu*/
        clear();
        print_frame();
        add_menu_label();
        for (; j < *menu_size; ++j) {       
            if (j == key_id) {
                init_pair(1, COLOR_RED, COLOR_BLACK);
                attron(COLOR_PAIR(1));
                mvprintw(j+4, 2, "%s", m_item[j].item_name); 
                attroff(COLOR_PAIR(1));
                if ( (char)(ch) == '\n') {
                    m_item[j].item_operation(); 
                }
            }
            else {
                mvprintw(j+4, 2, "%s", m_item[j].item_name); 
            }
        }
        add_designer_label();
        refresh();
        j = 0;
    }
}
static void ping_lkms1()
{
    system("xterm -hold -e ping -b 10.0.0.1&");
}
static void ping_lkms2()
{
    system("xterm -hold -e ping -b 10.0.0.2&");
}
static void ping_lkms3()
{
    system("xterm -hold -e ping -b 10.0.0.3&");
}
static void ping_lkms4()
{
    system("xterm -hold -e ping -b 10.0.0.4&");
}
static void ping_lkms5()
{
    system("xterm -hold -e ping -b 10.0.0.5&");
}
static void ping_lkms6()
{
    system("xterm -hold -e ping -b 10.0.0.6&");
}
static void ping_lkms7()
{
    system("xterm -hold -e ping -b 10.0.0.7&");
}
static void ping_lkms8()
{
    system("xterm -hold -e ping -b 10.0.0.8&");
}
static void monit_keys()
{   
    int j = 0;
    int letter;
    int key_id = 0;
    while ((letter = getch()) != KEY_LEFT) {
        switch(letter) {
        case KEY_UP:
                if (key_id <= 0)
                    key_id = *ports_num;
                else 
                    key_id -= 1;
            break;
        case KEY_DOWN:
                if (key_id >= *ports_num)
                    key_id = 0;
                else
                    key_id += 1;
            break;
        }
        for (; j < *ports_num; ++j) {       
            if (j == key_id) {
                init_pair(1, COLOR_RED, COLOR_BLACK);
                attron(COLOR_PAIR(1));
                mvprintw(j+5, 15, "%c", '-');
                mvprintw(j+5, 16, "%s", port[j].port_name); 
                attroff(COLOR_PAIR(1));
                if ( (char)(letter) == '\n') {
                    port[j].port_operation(); 
                }
            }
            else {
                mvprintw(j+5, 15, "%c", '-');
                mvprintw(j+5, 16, "%s", port[j].port_name); 
            }
        }
        refresh();
        j = 0;
    }
}

static void print_menu_items() 
{
    int i = 0;
    for (; i < *menu_size; ++i)
        mvprintw(i+4, 2, "%s", m_item[i].item_name); 
}   

static void print_port_items()
{   
    int i = 0;
    for (; i < *ports_num; ++i) {
        mvprintw(i+5, 15, "%c", '-');
        mvprintw(i+5, 16, "%s ", port[i].port_name );
    }
}

static void ping_broadcast()
{   
    system("xterm -hold -e ping -b 10.0.0.255&");
}

static void ping_vlan()
{   
    printw(" give address");
}

static menu_ports *init_ports(int *menu_size)
{
   static menu_ports ports[] = { {"lkms[1]",ping_lkms1},
                                 {"lkms[2]",ping_lkms2 },
                                 {"lkms[3]",ping_lkms3},
                                 {"lkms[4]",ping_lkms4},
                                 {"lkms[5]",ping_lkms5},
                                 {"lkms[6]",ping_lkms6},
                                 {"lkms[7]",ping_lkms7},
                                 {"lkms[8]",ping_lkms8},
                                };

    *ports_num = (sizeof(ports)/sizeof(menu_ports));
    return ports;
}

static void ping_port()
{   
    int size = 0;
    ports_num = &size;

    port = init_ports(ports_num);
    print_menu_items();
    add_designer_label();
    print_port_items();
    monit_keys();
    clear();
    print_frame();
    add_menu_label();
    print_menu_items();
    add_designer_label();
}

static menu_item *init_menu(int *menu_size)
{   
    /*TODO remember to use strncpy to copy item name to item_name attr inside menu_item struct*/
    static menu_item m_item[] = {  {"Ping broadcast", ping_broadcast},
                            {"Ping one port", ping_port},
                            {"Ping vlan", ping_vlan} };

    *menu_size = (sizeof(m_item)/sizeof(menu_item));
    return m_item;
}

static void print_frame()
{   
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    attron(COLOR_PAIR(2));
    char frame[] = "************************";

    mvprintw(0, 0, "%s", frame);
    int i = 0;
    for (; i < sizeof(frame); ++i) {
        mvprintw(i, 0, "%c", frame[i]);
    }
    mvprintw(i-2, 0, frame); 
    i = 0;
    for (; i < sizeof(frame); ++i) {
        mvprintw(i, sizeof(frame)-1, "%c", frame[i]); 
    }
    attroff(COLOR_PAIR(2));
}

static void add_menu_label()
{
    attron(COLOR_PAIR(3));
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    mvprintw(2, 16, "%s", "MENU");
    attroff(COLOR_PAIR(3));
}

static void add_designer_label()
{
    attron(COLOR_PAIR(4));
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    mvprintw(20, 7, "%s", "DESIGNED BY aK");
    attroff(COLOR_PAIR(4));
}

static void print_menu()
{   
    int array_size = 0;

    menu_size = &array_size;
    m_item = init_menu(menu_size);
    print_frame();
    add_menu_label();
    print_menu_items();
    add_designer_label();
    main_loop();
    endwin();
    attroff(COLOR_PAIR(1));
}

test_cases *init_test()
{
    /*if (curs_set(0) == ERR) {
        //printw("Turning off cursor not supported\n");
    }*/
    test_cases *test = malloc(sizeof(test_cases));
    test->print_menu = print_menu;
    initscr();
    if (has_colors() == FALSE) {
        endwin();
        printw("Terminal does not support color\n");
        exit(1);
    }
    raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    return test;
}

void dealloc_test(test_cases *test)
{
    free(test);
}



