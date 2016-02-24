#include "store.h"
#include "config.h"
#include "store-test.c"

struct lock {
    int id;
    int nameCount;
    char* names[MAX_NAME_LENGTH];
};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: distributed-db config-file\n");
        return 1;
    }

    store_init();
    store_test();
    struct node* thisNode = this_node(argv[1]);
    printf("id: %d, host: %s, port: %d\n", thisNode->id, thisNode->address, thisNode->port);
    printf("Id for ... %d\n", id(argv[1], "193.11.112.177", 9004)); //TODO oopsie failed
    printf("what\n");
}