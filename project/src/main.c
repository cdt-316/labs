#include <stdlib.h>
#include "store.h"
#include "config.h"
#include "store-test.c"
#include "network.h"

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

    config_init(argv[1]);
    store_init();
    store_test();

    struct node* thisNode;
    if (argc == 2)
    {
        thisNode = this_node();
    } else
    {
        thisNode = malloc(sizeof(struct node));
        strcpy(thisNode->address, argv[2]);
        thisNode->port = atoi(argv[3]);
        thisNode->id = get_node_id(thisNode->address, thisNode->port);
    }

    int nodeCount = node_count();
    printf("id: %d, host: %s, port: %d\n", thisNode->id, thisNode->address, thisNode->port);
    printf("nodeCount: %d\n", nodeCount);
    for (int i = 0; i < nodeCount; i++)
    {
        struct node* current = node_for_id(i);
        printf("Config| id: %d, host: %s, port: %d\n", current->id, current->address, current->port);
    }

    network_init(nodeCount, thisNode);
}