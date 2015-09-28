#include <traffic-visualizer.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ncurses.h>
#include <sys/time.h>

#define PACKETS_PER_PORT 100
#define PATH_LEN 50
#define PATH_START 0
#define STEP_TIME 0.016f


struct packet {
  int x;
  int port;
  int direction;
  int alive;
};

struct port {
  char name[16];  
  int rx_bitrate;
  int tx_bitrate;
  float rx_time;
  float tx_time;
  int active;
  struct packet packets[PACKETS_PER_PORT];
};



struct traffic_visualizer_priv {
  struct port * ports; 
  int num_ports;
  double current_time;
  double last_time;
  float accumulator;
  int rx_color_pair;
  int tx_color_pair;
  int offset_x;
  int offset_y;
};


static double ftime(void) {
  double time_in_mill;
  struct timeval t;
  gettimeofday(&t, NULL);
  time_in_mill = (t.tv_sec) * 1000 + (t.tv_usec) / 1000 ;
  return time_in_mill * 0.001;
}


static float bitrate_to_time(int bitrate) {
  return 272.0f / bitrate;
}


static int activate_port(struct traffic_visualizer * self, int i, const char * name) {
  struct port * port;
  int j;

  if (i < 0 || self->p->num_ports <= i) {
    return -EINVAL;
  }

  port = &self->p->ports[i];
  if (port->active) {
    return -EBUSY;
  }    

  strncpy(port->name, name, 16);
  port->active = 1;
  port->rx_bitrate = 0;
  port->tx_bitrate = 0;
  
  for (j = 0; j < PACKETS_PER_PORT; ++j) {
    struct packet * packet = &port->packets[i];  
    packet->x = 0.0f;
    packet->port = i;
    packet->direction = 0;
    packet->alive = 0; 
  }  

  return 0;
}


static int deactivate_port(struct traffic_visualizer * self, int i) {
  struct port * port;
  if (i < 0 || self->p->num_ports <= i) {
    return -EINVAL;
  }
  port = &self->p->ports[i];
  port->active = 0;
  return 0;
}


static int set_tx_bitrate(struct traffic_visualizer * self, int i, int tx_bitrate) {
  struct port * port;
  if (i < 0 || self->p->num_ports <= i) {
    return -EINVAL;
  }

  port = &self->p->ports[i];
  if (!port->active) {
    return -EINVAL;
  }    

  port->tx_bitrate = tx_bitrate;
  return 0;
}


static int get_tx_bitrate(struct traffic_visualizer * self, int i) {
  struct port * port;
  if (i < 0 || self->p->num_ports <= i) {
    return -EINVAL;
  }

  port = &self->p->ports[i];
  if (!port->active) {
    return -EINVAL;
  }    

  return port->tx_bitrate;
}


static int set_rx_bitrate(struct traffic_visualizer * self, int i, int rx_bitrate) {
  struct port * port;
  if (i < 0 || self->p->num_ports <= i) {
    return -EINVAL;
  }

  port = &self->p->ports[i];
  if (!port->active) {
    return -EINVAL;
  }    

  port->rx_bitrate = rx_bitrate;
  return 0;
}


static int get_rx_bitrate(struct traffic_visualizer * self, int i) {
  struct port * port;
  if (i < 0 || self->p->num_ports <= i) {
    return -EINVAL;
  }

  port = &self->p->ports[i];
  if (!port->active) {
    return -EINVAL;
  }    

  return port->rx_bitrate;
}


static void print_ports(struct port * ports, int num_ports, int offy, int offx) {
  int i = 0;
  for (i = 0; i < num_ports; ++i) {
    mvprintw(offy + i + 1, offx + 0, ports[i].name);
    mvprintw(offy + i + 1, offx + PATH_LEN, "|");
  } 
}


static void print_packets(struct packet * packets, int num_packets, int rx, int tx, int offy, int offx) {
  int i = 0;
  for (i = 0; i < num_packets; ++i) {
    struct packet * packet = &packets[i];
    if (packet->alive) {
      attron(COLOR_PAIR(packet->direction == 1 ? tx : rx));
      mvprintw(offy + packet->port + 1, offx + packet->x, packet->direction == 1 ? ">" : "<");
      attroff(COLOR_PAIR(packet->direction == 1 ? tx : tx));
    }  
  }
}

static void clear_printed_packets(struct packet * packets, int num_packets, int offy, int offx) {
  int i = 0;
  for (i = 0; i < num_packets; ++i) {
    struct packet * packet = &packets[i];
    if (packet->alive) {
      mvprintw(offy + packet->port + 1, offx + packet->x, " ");
    }  
  }
}




static int activate_packet(struct packet * packets, int num_packets, int port, int direction) {
  int i = 0;
  for (i = 0; i < num_packets; ++i) {
    struct packet * packet = &packets[i];
    if (!packet->alive) {
      packet->alive = 1;
      packet->port = port;
      packet->direction = direction >= 0 ? 1 : -1;
      packet->x = direction >= 0 ? PATH_START : PATH_START + PATH_LEN;
      return 0;
    }
  }
  return -ENOMEM;
}
   

static void step(struct traffic_visualizer * self, float delta) {
  int i = 0;
  for (i = 0; i < self->p->num_ports; ++i) {
    int j;
    struct port * port = &self->p->ports[i];   
    struct packet * packets = port->packets;

    if (!port->active) {
      continue;  
    } 

    port->rx_time += delta;
    port->tx_time += delta;

    if (port->rx_bitrate > 0 && port->rx_time > bitrate_to_time(port->rx_bitrate)) {
      port->rx_time = 0;
      activate_packet(packets, PACKETS_PER_PORT, i, -1); 
    }

    if (port->tx_bitrate > 0 && port->tx_time > bitrate_to_time(port->tx_bitrate)) {
      port->tx_time = 0;
      activate_packet(packets, PACKETS_PER_PORT, i, 1); 
    }

    for (j = 0; j < PACKETS_PER_PORT; ++j) {
      struct packet * packet = &packets[j];
      if (packet->alive) {
        packet->x += packet->direction;
        if (packet->x < PATH_START || PATH_START + PATH_LEN < packet->x) {
          packet->alive = 0;
        }
      }
    }
  }
}


static void render_with_delta(struct traffic_visualizer * self, float delta) {
  int i;
  self->p->accumulator += delta;
  if (self->p->accumulator < STEP_TIME) {
    return;
  }
  self->p->accumulator = 0.0f;
  for (i = 0; i < self->p->num_ports; ++i) {
    struct port * port = &self->p->ports[i];
    clear_printed_packets(port->packets, PACKETS_PER_PORT, self->p->offset_y, self->p->offset_x);
  }

  step(self, STEP_TIME); 

  for (i = 0; i < self->p->num_ports; ++i) {
    struct port * port = &self->p->ports[i];
    print_packets(port->packets, PACKETS_PER_PORT, self->p->rx_color_pair, self->p->tx_color_pair, self->p->offset_y, self->p->offset_x);
  }
  print_ports(self->p->ports, self->p->num_ports, self->p->offset_y, self->p->offset_x);
}


static void render(struct traffic_visualizer * self) {
  self->p->last_time = self->p->current_time;
  self->p->current_time = ftime();
  self->render_with_delta(self, (float)(self->p->current_time - self->p->last_time));
}


static void set_offset_yx(struct traffic_visualizer * self, int y, int x) {
  self->p->offset_y = y;
  self->p->offset_x = x;
}


struct traffic_visualizer * create_traffic_visualizer(int max_ports, int rx_color_pair, int tx_color_pair) {
  struct traffic_visualizer * self = malloc(sizeof *self);  
  self->p = malloc(sizeof *self->p);

  self->p->ports = malloc(sizeof(struct port) * max_ports);
  memset(self->p->ports, 0, sizeof(struct port) * max_ports);
  self->p->num_ports = max_ports;  
  self->p->last_time = ftime();
  self->p->current_time = ftime();
  self->p->tx_color_pair = tx_color_pair;
  self->p->rx_color_pair = rx_color_pair;
  self->p->offset_x = 0;
  self->p->offset_y = 0;

  self->activate_port = activate_port;
  self->deactivate_port = deactivate_port;
  self->get_rx_bitrate = get_rx_bitrate;
  self->set_rx_bitrate = set_rx_bitrate;
  self->get_tx_bitrate = get_tx_bitrate;
  self->set_tx_bitrate = set_tx_bitrate;
  self->render_with_delta = render_with_delta;
  self->render = render;
  self->set_offset_yx = set_offset_yx;
  return self;
}


void destroy_traffic_visualizer(struct traffic_visualizer * self) {
  if (!self) {
    return;
  }
  free(self->p->ports);  
  free(self->p);
  free(self);
}
