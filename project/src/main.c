#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "store.h"
#include "config.h"
#include "network.h"
#include "client.h"

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3)
    {
        printf("Usage: distributed-db config-file [address]\n");
        printf("Where 'address' can be (optionally) manually specified\n");
        printf("If not specified, then 'address' (and node) will be autodetected from the system and config file\n");
        return 1;
    }

    config_init(argv[1]);
    store_init();

    struct node* thisNode;
    if (argc == 2)
    {
        thisNode = this_node();
    } else
    {
        thisNode = malloc(sizeof(struct node));
        strcpy(thisNode->address, argv[2]);
        thisNode->id = node_id(thisNode->address);
    }

    for (int i = 0; i < node_count(); i++)
    {
        struct node* current = node_for_id(i);
        printf("Config| id: %d, host: %s\n", current->id, current->address);
    }
    printf("This Node| id: %d, host: %s\n", thisNode->id, thisNode->address);

    network_init(node_count(), thisNode);
    run_client();
}
