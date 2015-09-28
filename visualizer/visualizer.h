#ifndef VIS_H
    #define VIS_H
    #include "../semaphore/semop.h" /* semaphore operation */
    #include <traffic-visualizer.h>
/* struct responsible for exchanging messages between processes */
typedef struct shm_message shm_message;
/* struct responsible for visualizing traffic */
typedef struct visualizer {
    void(*visualize)(struct traffic_visualizer *vis);
    struct traffic_visualizer* (*create_visualizer)(void); 
    void *(*get_seg_addr)(void);
}visualizer;
/* constructor + destructor */
visualizer *init_visualizer(void);
void dealloc_visualizer(visualizer*);
//void start_visualizer(void);
#endif

/*ifndef is using here(and in every header file) basically for preventing multiple includes */
