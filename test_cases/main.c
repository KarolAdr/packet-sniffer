#include "test.h"

int main()
{   
    test_cases *test = init_test();
    test->print_menu();

    dealloc_test(test);
    return 0;
}
