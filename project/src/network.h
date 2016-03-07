//
// Created by mitch on 3/6/16.
//

#ifndef DISTRIBUTED_DB_NETWORK_H
#define DISTRIBUTED_DB_NETWORK_H

#include <sys/types.h>
#include <sys/socket.h>
#include "config.h"

struct connection {
    struct node* node;
    int socket;
};

/**
 * Connects to each possible node, and leaves a socket open for other nodes to connect to.
 * Maintains the new connections.
 */
void network_init(int nodeCount, struct node* this);

/**
 * Asks all other alive nodes if the requested resource is available for locking.
 * Returns a 0 if the resource is available, otherwise one
 */
int isRemotelyLocked(char* resource);

#endif //DISTRIBUTED_DB_NETWORK_H
