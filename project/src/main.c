#include <stdlib.h>
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
    if (argc != 2 && argc != 4)
    {
        printf("Usage: distributed-db config-file [address port]\n");
        printf("Where 'address' and 'port' can be (optionally) manually specified\n");
        printf("If not specified, then 'address' and 'port' will be autodetected from the system and config file\n");
        return 1;
    }

    struct node* thisNode;
    if (argc == 2)
    {
        thisNode = this_node(argv[1]);
    } else
    {
        thisNode = malloc(sizeof(struct node));
        strcpy(thisNode->address, argv[2]);
        thisNode->port = atoi(argv[3]);
        thisNode->id = id(argv[1], thisNode->address, thisNode->port);
    }

    store_init();
    store_test();
    printf("id: %d, host: %s, port: %d\n", thisNode->id, thisNode->address, thisNode->port);
}