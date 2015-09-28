#include "visualizer.h"
#include <ncurses.h>
#include <traffic-visualizer.h>

int main()
{   
    struct traffic_visualizer *t_vis;
    visualizer *vis = init_visualizer();
    t_vis = vis->create_visualizer();
    vis->visualize(t_vis);
    //start_visualizer();     
    printf("Return from visualizer\n"); 
    destroy_traffic_visualizer(t_vis);
    dealloc_visualizer(vis);
    return 0;
}

