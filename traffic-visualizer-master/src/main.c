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
