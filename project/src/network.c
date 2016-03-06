//
// Created by mitch on 3/6/16.
//

#include <malloc.h>
#include "network.h"

struct connection* connections;

void network_init(int nodeCount, struct node nodes[])
{
    connections = malloc(sizeof(struct connection) * nodeCount);

    for (int i = 0; i < nodeCount; i++)
    {
        //struct node* = node_for_id()
        int connectSocket = socket(AF_INET, SOCK_DGRAM, 0);
    }
}