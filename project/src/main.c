#include "store.h"
#include "test.c"

struct lock {
    int id;
    int nameCount;
    char* names[MAX_NAME_LENGTH];
};

int main()
{
    store_init();
    test();
}