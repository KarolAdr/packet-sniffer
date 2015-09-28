#include "spawner.h"

int main()
{
    spawner *analyzer_spawner = init_spawner();
    analyzer_spawner->get_netns();

    dealloc_spawner(analyzer_spawner);
    return 0;
}
