#ifndef __traffic_visualizer_h__
#define __traffic_visualizer_h__


/**
 * This is simple "traffic visualizer" "class"
 * that can be used to visualize network traffic
 * on network devices
 */
struct traffic_visualizer {
  /**
   * private data - you shouldn't care
   */
  struct traffic_visualizer_priv * p;

  /**
   * activate_port - activates port and assigns name to it. Activated port will
   *                 be displayed
   * @self: traffic_visualizer instance
   * @i: port to activate. From range 0 to max_ports - 1
   * @name: port name, which will be displayed
   * 
   * This function returns 0 if port was activated sucessfully.
   * If port is already active it returns -EBUSY and if i is out
   * of range it returns -EINVAL
   */
  int (*activate_port)(struct traffic_visualizer * self, int i, const char * name); 

  /**
   * deactivate_port - deactivates port. The port will no longer be displayed
   * @self: traffic_visualizer instance
   * @i: port to deactivate. From range 0 to max_ports - 1
   * 
   * This function returns 0 if port was deactivated sucessfully.
   * If i is out of range it returns -EINVAL
   */
  int (*deactivate_port)(struct traffic_visualizer * self, int i);

  /**
   * set_tx_bitrate - sets outgoing bitrate. This determines how many packets
   *                  will be "flying"
   * @self: traffic_visualizer instance
   * @i: port from range 0 to max_ports - 1
   * @tx_bitrate: bitrate in bytes per second
   * 
   * Returns 0 on success
   * If i is out of range it returns -EINVAL
   */
  int (*set_tx_bitrate)(struct traffic_visualizer * self, int i, int tx_bitrate);

  /**
   * get_tx_bitrate - returns outgoing bitrate. 
   * @self: traffic_visualizer instance
   * @i: port from range 0 to max_ports - 1
   * 
   * Returns bitrate on success
   * If i is out of range it returns -EINVAL
   */
  int (*get_tx_bitrate)(struct traffic_visualizer * self, int i);

  /**
   * set_rx_bitrate - sets incoming bitrate. This determines how many packets
   *                  will be "flying"
   * @self: traffic_visualizer instance
   * @i: port from range 0 to max_ports - 1
   * @rx_bitrate: bitrate in bytes per second
   * 
   * Returns 0 on success
   * If i is out of range it returns -EINVAL
   */
  int (*set_rx_bitrate)(struct traffic_visualizer * self, int i, int rx_bitrate);

  /**
   * get_rx_bitrate - returns incoming bitrate. This determines how many packets
   *                  are "flying"
   * @self: traffic_visualizer instance
   * @i: port from range 0 to max_ports - 1
   * 
   * Returns bitrate on success
   * If i is out of range it returns -EINVAL
   */
  int (*get_rx_bitrate)(struct traffic_visualizer * self, int i);

  /**
   * render_with_delta - advances animation by 'delta' seconds, and
   *                     renders it.
   * @self: traffic_visualizer instance
   * @delta: time since last render
   */
  void (*render_with_delta)(struct traffic_visualizer * self, float delta);

  
  /**
   * render - advances animation, and renders it
   * @self: traffic_visualizer instance
   */
  void (*render)(struct traffic_visualizer * self);

  /**
   * set_offset_yx - changes where traffic visualizer will be rendered 
   * @self: traffic_visualizer instance
   * @y: vertical offset
   * @x: horizontal offset
   */
  void (*set_offset_yx)(struct traffic_visualizer * self, int y, int x);
};


/**
 * create_traffic_visualizer - creates traffic visualized. Remember to destroy it
 * @max_ports: maximum number of ports that you want to display
 * @rx_color_pair: color pair to use for received packets. This is number that you pass as
 *                 a first parameter to init_pair
 * @tx_color_pair: color pair to use for transmitted packets. This is number that you pass as
 *                 a first parameter to init_pair
 */
struct traffic_visualizer * create_traffic_visualizer(int max_ports, int rx_color_pair, int tx_color_pair);


/**
 * destroy_traffic_visualizerl - destructor
 * @self: traffic_visualizer instance
 */
void destroy_traffic_visualizer(struct traffic_visualizer * self);


#endif
