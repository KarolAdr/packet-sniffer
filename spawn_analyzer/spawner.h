#ifndef SPAWNER_H
    #define PATH_NAME_SIZE 64
    #define PATH_TO_NAMESPACES "/var/run/netns/"
    #define NAMESPACE_NAME_SIZE sizeof(PATH_TO_NAMESPACES)
    #define PATH_ANALYZER "../analyzer"
    #define ANALYZER "analyzer"
typedef struct spawner {
    int(*spawn_analyzer)(void);
    void(*get_netns)(void);
}spawner;

spawner* init_spawner(void);
void dealloc_spawner(spawner*);
#endif
