# traffic-visualizer

This is a very simple ncurses "helper" that renders some kind of traffic visualizer. 
It can be used to visualize network traffic on network devices.

![traffic visualizer](/anim.gif?raw=true "Traffic visualizer")
#Building

```bash
cd build
make
```

When you built the library you can run example

```bash
./run.sh
```

#Usage

First include necessary headers

```c
#include <traffic-visualizer.h>
```

create traffic visualizer:

```c
struct traffic_visualizer * vis = create_traffic_visualizer(32, 1, 2);
```

1 and 2 are color pairs for received packets and transmitted packets respectively.
You should initialize them using functions from ncurses

```c
init_pair(1, COLOR_BLACK, COLOR_RED); //black font and red background for received packets
init_pair(2, COLOR_BLACK, COLOR_BLUE); //black font and blue background for transmitted packets
```

Add ports which will represent network devices. You can add port up to **max_ports**
value you passed to **create_traffic_visualizer**

```c
  vis->activate_port(vis, 0, "portA");
  vis->activate_port(vis, 1, "portB");
  vis->activate_port(vis, 2, "portC");
  vis->activate_port(vis, 3, "portD");
  //... up to max_ports
  vis->activate_port(vis, 31, "portD");
```

In your main animation loop you should call **render** to advance animation
and draw it

```c
  vis->render(vis); 
```

You can update bitrate (how fast packets go) in realtime by calling

```c
  //Let's set incoming and outgoing bitrates for port 0
  vis->set_rx_bitrate(vis, 0, 3000); //bitrate of incoming traffic
  vis->set_tx_bitrate(vis, 0, 2000); //bitrate of outgoing traffic
  //And incoming bitrate for port 1
  vis->set_rx_bitrate(vis, 1, 30000);
```

Always remember to clean up at the end of your program

```c
  destroy_traffic_visualizer(vis);
```

#Example ncurses app

To make things more clear check this example program which displays
simple traffic (randomly generated :))

```c
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <traffic-visualizer.h>


#define NUM_DEVS 32
#define RX_COLOR 1
#define TX_COLOR 2


int main(int argc, char ** argv) {
  int i;
  struct traffic_visualizer * vis = create_traffic_visualizer(NUM_DEVS, RX_COLOR, TX_COLOR);

  initscr();
  start_color();
  init_pair(RX_COLOR, COLOR_BLACK, COLOR_RED);
  init_pair(TX_COLOR, COLOR_BLACK, COLOR_BLUE);
  noecho();
  nodelay(stdscr, 1);

  srand(time(NULL));

  for (i = 0; i < NUM_DEVS; ++i) {
    vis->activate_port(vis, i, "lkms");
    vis->set_rx_bitrate(vis, i, rand() % 500);
    vis->set_tx_bitrate(vis, i, rand() % 500);
  }

  while (getch() != 'q') {
    vis->render(vis);

    vis->set_rx_bitrate(vis, 7, rand() % 20000);
    vis->set_tx_bitrate(vis, 7, rand() % 400);

    usleep(33000);
    refresh();
  }

  endwin();

  destroy_traffic_visualizer(vis);
  return 0;
}
```

#More

For better understanding please check comments in **traffic-visualizer.h**
and for complete undestanding see **traffic-visualizer.c**
