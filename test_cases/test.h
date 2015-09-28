#ifndef TEST_H
    #define TEST_H
typedef struct menu_item menu_item;
typedef struct test_cases {
    void(*print_menu)(void);
}test_cases;
test_cases *init_test(void);
void dealloc_test(test_cases*);
#endif
